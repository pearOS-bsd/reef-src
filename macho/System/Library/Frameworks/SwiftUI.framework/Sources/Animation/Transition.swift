// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Transition (Animation).

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

public protocol Transition {
    associatedtype Body: SwiftUI.View
    static var properties: SwiftUI.TransitionProperties { get }
    func _makeContentTransition(transition a0: inout SwiftUI._Transition_ContentTransition)
    func body(content a0: SwiftUI.PlaceholderContentView<Self>, phase a1: SwiftUI.TransitionPhase) -> Self.Body
}

extension SwiftUI.Transition {
    public static var properties: SwiftUI.TransitionProperties { _skeleton() }
    public func _makeContentTransition(transition a0: inout SwiftUI._Transition_ContentTransition) { _skeleton() }
    public func animation(_ a0: SwiftUI.Animation?) -> some SwiftUI.Transition { self }
    public func combined<A1>(with a0: A1) -> some SwiftUI.Transition where A1: SwiftUI.Transition { self }
}

extension SwiftUI.Transition where Self == SwiftUI.SlideTransition {
    public static var slide: SwiftUI.SlideTransition { _skeleton() }
}

extension SwiftUI.Transition where Self == SwiftUI.OpacityTransition {
    public static var opacity: SwiftUI.OpacityTransition { _skeleton() }
}

extension SwiftUI.Transition where Self == SwiftUI.MoveTransition {
    public static func move(edge a0: SwiftUI.Edge) -> SwiftUI.MoveTransition { _skeleton() }
}

extension SwiftUI.Transition where Self == SwiftUI.ScaleTransition {
    public static var scale: SwiftUI.ScaleTransition { _skeleton() }
    public static func scale(_ a0: Swift.Double, anchor a1: SwiftUI.UnitPoint) -> SwiftUI.ScaleTransition { _skeleton() }
}

extension SwiftUI.Transition where Self == SwiftUI.IdentityTransition {
    public static var identity: SwiftUI.IdentityTransition { _skeleton() }
}

extension SwiftUI.Transition where Self == SwiftUI.PushTransition {
    public static func push(from a0: SwiftUI.Edge) -> SwiftUI.PushTransition { _skeleton() }
}
