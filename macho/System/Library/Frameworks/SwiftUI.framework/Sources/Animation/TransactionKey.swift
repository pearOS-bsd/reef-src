// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TransactionKey (Animation).

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

public protocol TransactionKey {
    associatedtype Value
    static func _valuesEqual(_ a0: Self.Value, _ a1: Self.Value) -> Swift.Bool
    static var defaultValue: Self.Value { get }
}

extension SwiftUI.TransactionKey {
    public static func _valuesEqual(_ a0: Self.Value, _ a1: Self.Value) -> Swift.Bool { _skeleton() }
}
