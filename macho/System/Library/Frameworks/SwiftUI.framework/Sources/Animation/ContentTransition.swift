// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentTransition (Animation).

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

public struct ContentTransition {
    let kind: SwiftUI.ContentTransition.Kind
    public static var opacity: SwiftUI.ContentTransition { _skeleton() }
    public static var interpolate: SwiftUI.ContentTransition { _skeleton() }
    public static func numericText(countsDown a0: Swift.Bool) -> SwiftUI.ContentTransition { _skeleton() }
    public static func == (_ a0: SwiftUI.ContentTransition, _ a1: SwiftUI.ContentTransition) -> Swift.Bool { _skeleton() }
    public static func numericText(value a0: Swift.Double) -> SwiftUI.ContentTransition { _skeleton() }
    public static func symbolEffect<A>(_ a0: A, options a1: Symbols.SymbolEffectOptions) -> SwiftUI.ContentTransition where A: Symbols.ContentTransitionSymbolEffect, A: Symbols.SymbolEffect { _skeleton() }
    public static var identity: SwiftUI.ContentTransition { _skeleton() }
    enum Kind {
        case numericText(countsDown: Swift.Bool)
        case identity
        case opacity
        case interpolate
    }
}

extension SwiftUI.ContentTransition: Swift.Equatable {
}

extension SwiftUI.ContentTransition.Kind: Swift.Equatable {
}
