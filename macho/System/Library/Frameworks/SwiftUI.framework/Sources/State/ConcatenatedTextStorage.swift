// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ConcatenatedTextStorage (State).

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

class ConcatenatedTextStorage: SwiftUI.AnyTextStorage {
    let first: SwiftUI.Text
    let second: SwiftUI.Text
    override var plainString: Swift.String {
        func plain(_ text: SwiftUI.Text) -> Swift.String {
            switch text.storage {
            case .verbatim(let string): return string
            case .anyTextStorage(let storage): return storage.plainString
            }
        }
        return plain(first) + plain(second)
    }
    init(_ a0: SwiftUI.Text, _ a1: SwiftUI.Text) {
        first = a0
        second = a1
        super.init()
    }
    deinit {}
}
