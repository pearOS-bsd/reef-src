// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreferenceKey (Environment).

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

public protocol PreferenceKey {
    associatedtype Value
    static var _includesRemovedValues: Swift.Bool { get }
    static var _isReadableByHost: Swift.Bool { get }
    static func _preferenceValuesEqual(_ a0: Self.Value, _ a1: Self.Value) -> Swift.Bool
    static var defaultValue: Self.Value { get }
    static func reduce(value a0: inout Self.Value, nextValue a1: () -> Self.Value)
}

extension SwiftUI.PreferenceKey {
    public static var _includesRemovedValues: Swift.Bool { _skeleton() }
    public static var _isReadableByHost: Swift.Bool { _skeleton() }
    public static func _preferenceValuesEqual(_ a0: Self.Value, _ a1: Self.Value) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.PreferenceKey where Self.Value: Swift.Equatable {
    public static func _preferenceValuesEqual(_ a0: Self.Value, _ a1: Self.Value) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.PreferenceKey where Self.Value: Swift.ExpressibleByNilLiteral {
    public static var defaultValue: Self.Value { _skeleton() }
}
