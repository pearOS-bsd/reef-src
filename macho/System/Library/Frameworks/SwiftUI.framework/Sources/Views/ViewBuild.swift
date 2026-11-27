// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewBuild: views and modifiers made into elements, through the body nodes that keep their state.

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

/// Makes views and modifiers into elements: primitive views build their own, representables wrap their platform view,
/// other views build their body under a body node keyed by the path and the view type (reused while nothing changed).
/// Debugging through the user defaults: ReefSwiftUITrace (log bodies, invalidations, property layouts) and
/// ReefSwiftUIDebug (comma-separated flags: noinstall, noobserve, noreuse, skip=TypePrefix).
enum ViewBuild {
    static var debugFlags: Swift.Set<Swift.String> = {
        let value = UserDefaults.standard.string(forKey: "ReefSwiftUIDebug") ?? ""
        return Swift.Set(value.split(separator: ",").map(String.init))
    }()
    static var debugSkips: [Swift.String] = debugFlags.compactMap { flag in
        flag.hasPrefix("skip=") ? String(flag.dropFirst(5)) : nil
    }
    static var tracing: Swift.Bool = UserDefaults.standard.bool(forKey: "ReefSwiftUITrace")
    static var reported: Swift.Set<Swift.ObjectIdentifier> = []

    /// log once per type that a kind of value ("view", "modifier") is not drawn yet
    static func reportUnhandled(_ a0: Any.Type, _ a1: Swift.String) {
        guard reported.insert(ObjectIdentifier(a0)).inserted else { return }
        NSLog("SwiftUI: %@ %@ is not drawn yet", a1, String(reflecting: a0))
    }

    static func isSkipped(_ a0: Any.Type) -> Swift.Bool {
        if debugSkips.isEmpty { return false }
        let name = String(describing: a0)
        return debugSkips.contains { name.hasPrefix($0) }
    }

    /// the elements of view a0
    static func build<A>(_ a0: A, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A: SwiftUI.View {
        if isSkipped(A.self) { return [] }
        if let primitive = a0 as? any SwiftUI.PrimitiveView { return primitive._buildElements(a1) }
        if let representable = a0 as? any SwiftUI.NSViewRepresentable { return SwiftUI.Representables.build(representable, a1) }
        if let representable = a0 as? any SwiftUI.NSViewControllerRepresentable {
            return SwiftUI.Representables.build(representable, a1)
        }
        if A.Body.self == Never.self {
            reportUnhandled(A.self, "view")
            return []
        }
        let node = a1.scope.child(SwiftUI.BodyNode.Key(parts: a1.path + [AnyHashable(SwiftUI.TypeKey(type: A.self))]), type: A.self)
        if let elements = reuse(node, a0, a1) { return elements }
        let body = node.body(of: a0, environment: a1.environment)
        var inner = a1
        inner.scope = node
        inner.path = []
        let host = a1.host
        let start = host.shortcuts.count
        host.openGroups.append([])
        let elements = node.makeBuildReader().track { build(body, inner) }
        if let group = host.openGroups.popLast(), !group.isEmpty {
            host.finishedGroups.insert(group, at: 0)
        }
        node.built = SwiftUI.BuiltElements(elements: elements, context: inner, shortcuts: host.shortcuts(since: start))
        node.needsBuild = false
        return elements
    }

    /// the elements built last time, when the node, the view and the environment it read did not change
    static func reuse<A>(_ a0: SwiftUI.BodyNode, _ a1: A, _ a2: SwiftUI.BuildContext) -> [SwiftUI.Element]? where A: SwiftUI.View {
        guard let built = a0.built, !a0.needsBuild, !a0.isDirty, !a0.isAnimating else { return nil }
        let host = a2.host
        guard built.generation != host.generation, !debugFlags.contains("noreuse"), built.matches(a2) else { return nil }
        if a1 is any SwiftUI.Animatable { return nil }
        guard let last = a0.lastValue as? A, SwiftUI.GraphCompare.viewEqual(last, a1) else { return nil }
        guard !a0.environmentChanged(a2.environment), built.environment.isSame(as: a2.environment, customOnly: false) else {
            return nil
        }
        a0.markVisited(host.generation)
        host.shortcuts.append(contentsOf: built.shortcuts)
        built.restore(host.generation)
        return built.elements
    }

    /// modifier a0 with its animatable data presented through its body node (when it has some)
    static func presented<A>(_ a0: A, _ a1: SwiftUI.BuildContext) -> A where A: SwiftUI.ViewModifier {
        guard let animatable = a0 as? any SwiftUI.Animatable, hasAnimatableData(animatable) else { return a0 }
        let node = a1.scope.child(SwiftUI.BodyNode.Key(parts: a1.path + [AnyHashable(SwiftUI.TypeKey(type: A.self))]), type: A.self)
        var value = a0
        node.presentAnimatable(&value, transaction: a1.host.transaction, frameOwner: a1.scope)
        return value
    }

    private static func hasAnimatableData<A: SwiftUI.Animatable>(_ a0: A) -> Swift.Bool {
        A.AnimatableData.self != SwiftUI.EmptyAnimatableData.self
    }

    /// the elements of content a2 with modifier a0 applied: primitive modifiers wrap the content themselves, others
    /// build their body with the content reachable through the context
    static func apply<A>(_ a0: A, _ a1: SwiftUI.BuildContext, content a2: (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] where A: SwiftUI.ViewModifier {
        return withoutActuallyEscaping(a2) { content in
            var contentContext = a1
            contentContext.path.append(AnyHashable("content"))
            if isSkipped(A.self) { return content(contentContext) }
            let modifier = presented(a0, a1)
            if let primitive = modifier as? any SwiftUI.PrimitiveModifier {
                return primitive._apply(a1) { context in
                    var context = context
                    context.path.append(AnyHashable("modified"))
                    return content(context)
                }
            }
            if A.Body.self == Never.self {
                reportUnhandled(A.self, "modifier")
                return content(contentContext)
            }
            let node = a1.scope.child(SwiftUI.BodyNode.Key(parts: a1.path + [AnyHashable(SwiftUI.TypeKey(type: A.self))]), type: A.self)
            let body = node.modifierBody(of: modifier, environment: a1.environment)
            var inner = a1
            inner.scope = node
            inner.path = []
            inner.modifierContents[ObjectIdentifier(A.self)] = SwiftUI.ModifierContent(build: content, context: contentContext)
            return build(body, inner)
        }
    }
}
