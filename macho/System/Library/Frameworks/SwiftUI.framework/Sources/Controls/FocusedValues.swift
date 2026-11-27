// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedValues (Controls).

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

public struct FocusedValues {
    var values: [Swift.ObjectIdentifier: Any]
    func holdsSameValues(`as` a0: SwiftUI.FocusedValues) -> Swift.Bool { _skeleton() }
    public subscript<A>(_ a0: A.Type) -> A.Value? where A: SwiftUI.FocusedValueKey {
        get { _skeleton() }
        set { _skeleton() }
    }
    public static func == (_ a0: SwiftUI.FocusedValues, _ a1: SwiftUI.FocusedValues) -> Swift.Bool { _skeleton() }
    static var current: SwiftUI.FocusedValues { _skeleton() }
}

extension SwiftUI.FocusedValues: Swift.Equatable {
}
