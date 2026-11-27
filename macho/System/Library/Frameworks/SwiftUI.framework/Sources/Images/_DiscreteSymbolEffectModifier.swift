// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _DiscreteSymbolEffectModifier (Images).

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

public struct _DiscreteSymbolEffectModifier<A> where A: Swift.Equatable {
    public var effect: Symbols.SymbolEffectConfiguration
    public var options: Symbols.SymbolEffectOptions
    public var value: A
    public init(effect a0: Symbols.SymbolEffectConfiguration, options a1: Symbols.SymbolEffectOptions, value a2: A) { _skeleton() }
    public func body(content a0: SwiftUI._ViewModifier_Content<SwiftUI._DiscreteSymbolEffectModifier<A>>) -> some SwiftUI.View { _skeleton() }
}

extension SwiftUI._DiscreteSymbolEffectModifier: SwiftUI.ViewModifier {
}
