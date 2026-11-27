// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewFields (Views).

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

struct ViewFields {
    let entries: [SwiftUI.ViewFields.Entry]
    let isComplete: Swift.Bool
    static func fields(of a0: Any.Type) -> SwiftUI.ViewFields { _skeleton() }
    static func collect(_ a0: Any.Type, base a1: Swift.Int, into a2: inout [SwiftUI.ViewFields.Entry]) -> Swift.Bool { _skeleton() }
    static var cache: [Swift.ObjectIdentifier: SwiftUI.ViewFields] { _skeleton() }
    struct Entry {
        let offset: Swift.Int
        let size: Swift.Int
        let binding: (any SwiftUI.BindingField.Type)?
    }
}
