// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyTransition (Animation).

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

public struct AnyTransition {
    public var box: SwiftUI.AnyTransitionBox
    public static func offset(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) -> SwiftUI.AnyTransition { _skeleton() }
    public static func scale(scale a0: CoreGraphics.CGFloat, anchor a1: SwiftUI.UnitPoint) -> SwiftUI.AnyTransition { _skeleton() }
    public static var identity: SwiftUI.AnyTransition { _skeleton() }
    public static var opacity: SwiftUI.AnyTransition { _skeleton() }
    public static var scale: SwiftUI.AnyTransition { _skeleton() }
    public static var slide: SwiftUI.AnyTransition { _skeleton() }
    public static func move(edge a0: SwiftUI.Edge) -> SwiftUI.AnyTransition { _skeleton() }
    public static func modifier<A>(active a0: A, identity a1: A) -> SwiftUI.AnyTransition where A: SwiftUI.ViewModifier { _skeleton() }
    public func combined(with a0: SwiftUI.AnyTransition) -> SwiftUI.AnyTransition { _skeleton() }
    public func animation(_ a0: SwiftUI.Animation?) -> SwiftUI.AnyTransition { _skeleton() }
    public init<A>(_ a0: A) where A: SwiftUI.Transition { _skeleton() }
    public static func asymmetric(insertion a0: SwiftUI.AnyTransition, removal a1: SwiftUI.AnyTransition) -> SwiftUI.AnyTransition { _skeleton() }
    public static func push(from a0: SwiftUI.Edge) -> SwiftUI.AnyTransition { _skeleton() }
}
