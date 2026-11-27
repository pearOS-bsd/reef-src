// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Animatable (Animation).

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

public protocol Animatable {
    associatedtype AnimatableData: SwiftUI.VectorArithmetic
    static func _makeAnimatable(value a0: inout SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._GraphInputs)
    var animatableData: Self.AnimatableData { get set }
}

extension SwiftUI.Animatable {
    public static func _makeAnimatable(value a0: inout SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._GraphInputs) { _skeleton() }
}

extension SwiftUI.Animatable where Self.AnimatableData == SwiftUI.EmptyAnimatableData {
    public var animatableData: SwiftUI.EmptyAnimatableData {
        get { _skeleton() }
        set { _skeleton() }
    }
    public static func _makeAnimatable(value a0: inout SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._GraphInputs) { _skeleton() }
}
