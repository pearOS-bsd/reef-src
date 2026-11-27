// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedValue (Controls).

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

public struct FocusedValue<A> {
    let keyPath: Swift.KeyPath<SwiftUI.FocusedValues, A?>
    public init(_ a0: Swift.KeyPath<SwiftUI.FocusedValues, A?>) { _skeleton() }
    public var content: SwiftUI.FocusedValue<A>.Content { _skeleton() }
    public var wrappedValue: A? { _skeleton() }
    @frozen public enum Content {
        case keyPath(Swift.KeyPath<SwiftUI.FocusedValues, A?>)
        case value(A?)
    }
}

extension SwiftUI.FocusedValue: SwiftUI.DynamicProperty {
}
