// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PickerOption (Controls).

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

struct PickerOption {
    let kind: SwiftUI.PickerOption.Kind
    let tag: Swift.AnyHashable
    let title: Swift.String
    let image: NSImage?
    let view: SwiftUI.AnyView
    static var separator: SwiftUI.PickerOption { _skeleton() }
    enum Kind {
        case option
        case separator
        case header
    }
}

extension SwiftUI.PickerOption.Kind: Swift.Hashable {
}

extension SwiftUI.PickerOption.Kind: Swift.Equatable {
}
