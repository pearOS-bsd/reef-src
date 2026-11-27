// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Inflection (Text).

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

enum Inflection {
    static func inflects(_ a0: Swift.String) -> Swift.Bool { _skeleton() }
    static func number(_ a0: Swift.String) -> Swift.Double? { _skeleton() }
    static func noun(_ a0: Swift.String, singular a1: Swift.Bool) -> Swift.String { _skeleton() }
    static func inflected(_ a0: Swift.String) -> Swift.String { _skeleton() }
    static func applyingExtendedAttributes(_ a0: Swift.String) -> Swift.String { _skeleton() }
    static var verbs: [Swift.String: (singular: Swift.String, plural: Swift.String)] { _skeleton() }
    static var unchanging: Swift.Set<Swift.String> { _skeleton() }
    struct Token {
        var text: Swift.String
        let isWord: Swift.Bool
    }
}
