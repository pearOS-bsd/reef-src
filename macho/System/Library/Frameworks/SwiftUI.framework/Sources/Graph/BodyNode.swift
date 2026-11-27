// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BodyNode (Graph).

import Foundation
import AppKit
import CoreGraphics
import CoreText
import Combine
import Observation
import CoreTransferable
import UniformTypeIdentifiers
import Symbols
import DeveloperToolsSupport
import Metal
import os

class BodyNode: SwiftUI.GraphRule {
    let type: Any.Type
    var generation: Swift.Int = 0
    var children: [SwiftUI.BodyNode.Key: SwiftUI.BodyNode] = [:]
    var persistent: [SwiftUI.BodyNode.Key: AnyObject] = [:]
    var persistentGeneration: [SwiftUI.BodyNode.Key: Swift.Int] = [:]
    var persistentSequence: [SwiftUI.BodyNode.Key: Swift.Int] = [:]
    var storage: [Swift.Int: AnyObject] = [:]
    var lastValue: Any? = nil
    var lastBody: Any? = nil
    var bodyRuns: Swift.Int = 0
    var environmentReads: [(SwiftUI.EnvironmentValues) -> Swift.Bool] = []
    var animationSlot: AnyObject? = nil
    weak var parentNode: SwiftUI.BodyNode?
    var built: SwiftUI.BuiltElements? = nil
    var needsBuild: Swift.Bool = true
    var buildReader: SwiftUI.BuildReader? = nil
    override func invalidate(_ a0: SwiftUI.Transaction) {
        if SwiftUI.ViewBuild.tracing {
            NSLog("SwiftUI: invalidate %@", String(reflecting: type))
        }
        super.invalidate(a0)
    }
    /// let an animatable value see the transaction (its animation runs from the frame owner)
    final func presentAnimatable<A>(_ a0: inout A, transaction a1: SwiftUI.Transaction, frameOwner a2: SwiftUI.BodyNode?) {
        guard var animatable = a0 as? any SwiftUI.Animatable else { return }
        presentAnimatableOpened(&animatable, transaction: a1, frameOwner: a2 ?? self)
        if let value = animatable as? A { a0 = value }
    }
    final func presentAnimatableOpened<A>(_ a0: inout A, transaction a1: SwiftUI.Transaction, frameOwner a2: SwiftUI.BodyNode) where A: SwiftUI.Animatable { _skeleton() }
    final func animatableSlot<A>(_ a0: A, frameOwner a1: SwiftUI.BodyNode) -> SwiftUI.AnimatableSlot<A> where A: SwiftUI.VectorArithmetic { _skeleton() }
    final var isAnimating: Swift.Bool {
        guard let slot = animationSlot as? any SwiftUI.AnimatingSlot else { return false }
        return slot.isRunning
    }
    /// the object kept under key a0 (made by a1 the first time or when the kept one has another type, which is torn down)
    final func persistentObject<A>(_ a0: SwiftUI.BodyNode.Key, _ a1: () -> A) -> A where A: AnyObject {
        persistentGeneration[a0] = host?.generation ?? 0
        if let object = persistent[a0] as? A { return object }
        if let old = persistent[a0], let teardown = old as? any SwiftUI.GraphTeardown { teardown.teardown() }
        let object = a1()
        persistent[a0] = object
        SwiftUI.BodyNode.sequence += 1
        persistentSequence[a0] = SwiftUI.BodyNode.sequence
        return object
    }
    /// the child node under key a0, made again when the view type changed
    final func child(_ a0: SwiftUI.BodyNode.Key, type a1: Any.Type) -> SwiftUI.BodyNode {
        if let node = children[a0], node.type == a1 {
            node.generation = host?.generation ?? 0
            return node
        }
        if let old = children[a0] { old.teardownAll() }
        let node = SwiftUI.BodyNode(type: a1, host: host)
        node.generation = host?.generation ?? 0
        node.parentNode = self
        children[a0] = node
        return node
    }
    /// the body of view a0: the last one when the view and the environment it read did not change, else made again with
    /// its dynamic properties installed, under this node's tracking
    final func body<A>(of a0: A, environment a1: SwiftUI.EnvironmentValues) -> A.Body where A: SwiftUI.View {
        var value = a0
        presentAnimatable(&value, transaction: host?.transaction ?? SwiftUI.Transaction(), frameOwner: nil)
        let current = value
        if !isDirty, let last = lastValue as? A, let body = lastBody as? A.Body, SwiftUI.GraphCompare.viewEqual(last, current),
           !environmentChanged(a1) {
            return body
        }
        if SwiftUI.ViewBuild.tracing {
            let reason: String
            if isDirty {
                reason = "dirty"
            } else if let last = lastValue as? A {
                reason = SwiftUI.GraphCompare.viewEqual(last, current) ? "environment" : "value"
            } else {
                reason = "new"
            }
            NSLog("SwiftUI: body of %@ (%@) %@ -> %@", String(reflecting: A.self), reason,
                  SwiftUI.DynamicPropertyLayout.describeFields(A.self), String(reflecting: A.Body.self))
        }
        var view = current
        let body = track { () -> A.Body in
            if !SwiftUI.ViewBuild.debugFlags.contains("noinstall") {
                SwiftUI.DynamicPropertyLayout.install(&view, node: self, environment: a1)
            }
            if SwiftUI.ViewBuild.debugFlags.contains("noobserve") { return view.body }
            return observing { view.body }
        }
        lastValue = current
        lastBody = body
        bodyRuns += 1
        return body
    }
    /// the reader whose dependencies make this node build again
    final func makeBuildReader() -> SwiftUI.BuildReader {
        if let reader = buildReader { return reader }
        let reader = SwiftUI.BuildReader()
        reader.node = self
        reader.host = host
        buildReader = reader
        return reader
    }
    /// whether one of the environment values the body read changed
    final func environmentChanged(_ a0: SwiftUI.EnvironmentValues) -> Swift.Bool {
        for read in environmentReads where read(a0) { return true }
        return false
    }
    /// keep this subtree alive in generation a0
    final func markVisited(_ a0: Swift.Int) {
        generation = a0
        for key in persistent.keys { persistentGeneration[key] = a0 }
        for child in children.values { child.markVisited(a0) }
    }
    /// the body of modifier a0, as body(of:environment:) does for views (no trace)
    final func modifierBody<A>(of a0: A, environment a1: SwiftUI.EnvironmentValues) -> A.Body where A: SwiftUI.ViewModifier {
        var value = a0
        presentAnimatable(&value, transaction: host?.transaction ?? SwiftUI.Transaction(), frameOwner: nil)
        let current = value
        if !isDirty, let last = lastValue as? A, let body = lastBody as? A.Body, SwiftUI.GraphCompare.viewEqual(last, current),
           !environmentChanged(a1) {
            return body
        }
        var modifier = current
        let body = track { () -> A.Body in
            SwiftUI.DynamicPropertyLayout.install(&modifier, node: self, environment: a1)
            return observing { modifier.body(content: SwiftUI._ViewModifier_Content<A>()) }
        }
        lastValue = current
        lastBody = body
        return body
    }
    /// run a0 tracking the observable properties it reads: a change invalidates this node on the main thread
    final func observing<A>(_ a0: () -> A) -> A {
        withObservationTracking(a0, onChange: { [weak self] in
            if Thread.isMainThread {
                self?.invalidate(SwiftUI.Transaction())
            } else {
                DispatchQueue.main.async { [weak self] in
                    self?.invalidate(SwiftUI.Transaction())
                }
            }
        })
    }
    /// a dirty node and its ancestors must build again
    override var isDirty: Swift.Bool {
        get { super.isDirty }
        set {
            super.isDirty = newValue
            guard newValue else { return }
            needsBuild = true
            var node = parentNode
            while let current = node {
                current.needsBuild = true
                node = current.parentNode
            }
        }
    }
    final func markVisited(prefix a0: [Swift.AnyHashable], _ a1: Swift.Int) { _skeleton() }
    final func leave(prefix a0: [Swift.AnyHashable]) { _skeleton() }
    final func leaveAll() {
        needsBuild = true
        for key in persistentKeysLatestFirst() {
            if let presence = persistent[key] as? any SwiftUI.GraphPresence { presence.leave() }
        }
        for child in children.values { child.leaveAll() }
    }
    /// the persistent keys, the most recently made first
    private final func persistentKeysLatestFirst() -> [SwiftUI.BodyNode.Key] {
        var keys = Array(persistent.keys)
        keys.sort { (persistentSequence[$1] ?? 0) < (persistentSequence[$0] ?? 0) }
        return keys
    }
    /// stop tracking, tear down the persistent objects (latest first) and the slots, then the children
    final func teardownAll() {
        for source in sources { source.readers.removeValue(forKey: ObjectIdentifier(self)) }
        sources = []
        built = nil
        if let reader = buildReader {
            for source in reader.sources { source.readers.removeValue(forKey: ObjectIdentifier(reader)) }
            reader.sources = []
        }
        for key in persistentKeysLatestFirst() {
            if let teardown = persistent[key] as? any SwiftUI.GraphTeardown { teardown.teardown() }
        }
        for object in storage.values {
            if let teardown = object as? any SwiftUI.GraphTeardown { teardown.teardown() }
        }
        persistent = [:]
        persistentGeneration = [:]
        persistentSequence = [:]
        storage = [:]
        for child in children.values { child.teardownAll() }
        children = [:]
    }
    /// the object kept for a dynamic property in slot a0, made the first time
    final func slot<A>(_ a0: Swift.Int, _ a1: () -> A) -> A where A: AnyObject {
        if let object = storage[a0] as? A { return object }
        let object = a1()
        storage[a0] = object
        return object
    }
    /// drop the children and persistent objects not visited in generation a0 (latest first), then sweep the children
    final func sweep(_ a0: Swift.Int) {
        for (key, child) in children where child.generation != a0 {
            child.teardownAll()
            children[key] = nil
        }
        var stale: [SwiftUI.BodyNode.Key] = []
        for key in persistent.keys where persistentGeneration[key] != a0 { stale.append(key) }
        stale.sort { (persistentSequence[$1] ?? 0) < (persistentSequence[$0] ?? 0) }
        for key in stale {
            if let teardown = persistent[key] as? any SwiftUI.GraphTeardown { teardown.teardown() }
            persistent[key] = nil
            persistentGeneration[key] = nil
            persistentSequence[key] = nil
        }
        for child in children.values { child.sweep(a0) }
    }
    init(type a0: Any.Type, host a1: SwiftUI.ViewHost?) {
        type = a0
        super.init()
        isDirty = true
        host = a1
    }
    static var sequence: Swift.Int = 0
    deinit {}
    struct Key {
        var parts: [Swift.AnyHashable]
    }
}

extension SwiftUI.BodyNode.Key: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { a0.parts == a1.parts }
}

extension SwiftUI.BodyNode.Key: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { a0.combine(parts) }
}
