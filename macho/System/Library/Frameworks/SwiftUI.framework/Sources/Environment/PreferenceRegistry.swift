// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreferenceRegistry (Environment).

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

enum PreferenceRegistry {
    static func register<A>(_ a0: A.Type) -> Swift.ObjectIdentifier where A: SwiftUI.PreferenceKey { _skeleton() }
    static func value(_ a0: Swift.ObjectIdentifier, `in` a1: SwiftUI.Element) -> Any? { _skeleton() }
    static func value<A>(_ a0: A.Type, `in` a1: [SwiftUI.Element]) -> A.Value where A: SwiftUI.PreferenceKey { _skeleton() }
    static var reducers: [Swift.ObjectIdentifier: SwiftUI.PreferenceRegistry.Reducer] { _skeleton() }
    struct Reducer {
        let defaultValue: () -> Any
        let reduce: (inout Any, Any) -> ()
    }
}
