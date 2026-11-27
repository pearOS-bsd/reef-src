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
    /// the comparable fields of a0 (cached); incomplete when a0 is not a struct with fields
    static func fields(of a0: Any.Type) -> SwiftUI.ViewFields {
        if let known = cache[ObjectIdentifier(a0)] { return known }
        var entries: [SwiftUI.ViewFields.Entry] = []
        let complete = collect(a0, base: 0, into: &entries)
        let fields = SwiftUI.ViewFields(entries: entries, isComplete: complete)
        cache[ObjectIdentifier(a0)] = fields
        return fields
    }
    /// the fields of struct a0 at byte offset a1: bindings as they are, nested structs field by field, the rest as bytes
    static func collect(_ a0: Any.Type, base a1: Swift.Int, into a2: inout [SwiftUI.ViewFields.Entry]) -> Swift.Bool {
        guard _reefMetadataKind(a0) == 0x200 else { return false }
        var fields: [(Swift.Int, Any.Type)] = []
        guard _reefForEachField(of: a0, { _, offset, type in fields.append((offset, type)); return true }), !fields.isEmpty else {
            return false
        }
        for (offset, type) in fields {
            if let binding = type as? any SwiftUI.BindingField.Type {
                a2.append(Entry(offset: a1 + offset, size: 0, binding: binding))
                continue
            }
            var nested: [SwiftUI.ViewFields.Entry] = []
            if collect(type, base: a1 + offset, into: &nested) {
                a2.append(contentsOf: nested)
            } else {
                a2.append(Entry(offset: a1 + offset, size: _reefSize(of: type), binding: nil))
            }
        }
        return true
    }
    static var cache: [Swift.ObjectIdentifier: SwiftUI.ViewFields] = [:]
    struct Entry {
        let offset: Swift.Int
        let size: Swift.Int
        let binding: (any SwiftUI.BindingField.Type)?
    }
}
