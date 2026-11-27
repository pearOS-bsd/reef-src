// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PickerOptions (Controls).

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

enum PickerOptions {
    static func entries<A>(_ a0: A, selection a1: Any.Type, _ a2: SwiftUI.EnvironmentValues) -> [SwiftUI.PickerOption] where A: SwiftUI.View { _skeleton() }
    static func walk(_ a0: Any, selection a1: Any.Type, into a2: inout [SwiftUI.PickerOption]) { _skeleton() }
    static var kinds: [Swift.ObjectIdentifier: SwiftUI.PickerOptions.Kind] { _skeleton() }
    enum Kind {
        case tagged
        case divider
        case section
        case tuple
        case forEach
        case anyView
        case optional
        case conditional
        case modified
        case group
        case other
    }
}

extension SwiftUI.PickerOptions.Kind: Swift.Hashable {
}

extension SwiftUI.PickerOptions.Kind: Swift.Equatable {
}
