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
    let type: Any.Type = _skeletonValue()
    var generation: Swift.Int = _skeletonValue()
    var children: [SwiftUI.BodyNode.Key: SwiftUI.BodyNode] = _skeletonValue()
    var persistent: [SwiftUI.BodyNode.Key: AnyObject] = _skeletonValue()
    var persistentGeneration: [SwiftUI.BodyNode.Key: Swift.Int] = _skeletonValue()
    var persistentSequence: [SwiftUI.BodyNode.Key: Swift.Int] = _skeletonValue()
    var storage: [Swift.Int: AnyObject] = _skeletonValue()
    var lastValue: Any? = _skeletonValue()
    var lastBody: Any? = _skeletonValue()
    var bodyRuns: Swift.Int = _skeletonValue()
    var environmentReads: [(SwiftUI.EnvironmentValues) -> Swift.Bool] = _skeletonValue()
    var animationSlot: AnyObject? = _skeletonValue()
    weak var parentNode: SwiftUI.BodyNode?
    var built: SwiftUI.BuiltElements? = _skeletonValue()
    var needsBuild: Swift.Bool = _skeletonValue()
    var buildReader: SwiftUI.BuildReader? = _skeletonValue()
    override func invalidate(_ a0: SwiftUI.Transaction) { _skeleton() }
    final func presentAnimatable<A>(_ a0: inout A, transaction a1: SwiftUI.Transaction, frameOwner a2: SwiftUI.BodyNode?) { _skeleton() }
    final func presentAnimatableOpened<A>(_ a0: inout A, transaction a1: SwiftUI.Transaction, frameOwner a2: SwiftUI.BodyNode) where A: SwiftUI.Animatable { _skeleton() }
    final func animatableSlot<A>(_ a0: A, frameOwner a1: SwiftUI.BodyNode) -> SwiftUI.AnimatableSlot<A> where A: SwiftUI.VectorArithmetic { _skeleton() }
    final var isAnimating: Swift.Bool { _skeleton() }
    final func persistentObject<A>(_ a0: SwiftUI.BodyNode.Key, _ a1: () -> A) -> A where A: AnyObject { _skeleton() }
    final func child(_ a0: SwiftUI.BodyNode.Key, type a1: Any.Type) -> SwiftUI.BodyNode { _skeleton() }
    final func body<A>(of a0: A, environment a1: SwiftUI.EnvironmentValues) -> A.Body where A: SwiftUI.View { _skeleton() }
    final func makeBuildReader() -> SwiftUI.BuildReader { _skeleton() }
    final func environmentChanged(_ a0: SwiftUI.EnvironmentValues) -> Swift.Bool { _skeleton() }
    final func markVisited(_ a0: Swift.Int) { _skeleton() }
    final func modifierBody<A>(of a0: A, environment a1: SwiftUI.EnvironmentValues) -> A.Body where A: SwiftUI.ViewModifier { _skeleton() }
    final func observing<A>(_ a0: () -> A) -> A { _skeleton() }
    override var isDirty: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    final func markVisited(prefix a0: [Swift.AnyHashable], _ a1: Swift.Int) { _skeleton() }
    final func leave(prefix a0: [Swift.AnyHashable]) { _skeleton() }
    final func leaveAll() { _skeleton() }
    final func teardownAll() { _skeleton() }
    final func slot<A>(_ a0: Swift.Int, _ a1: () -> A) -> A where A: AnyObject { _skeleton() }
    final func sweep(_ a0: Swift.Int) { _skeleton() }
    init(type a0: Any.Type, host a1: SwiftUI.ViewHost?) { _skeleton() }
    static var sequence: Swift.Int { _skeleton() }
    deinit {}
    struct Key {
        var parts: [Swift.AnyHashable]
    }
}

extension SwiftUI.BodyNode.Key: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.BodyNode.Key: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}
