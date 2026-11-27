// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to __C_NSPasteboardType.

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

extension NSPasteboard.PasteboardType: Swift._HasCustomAnyHashableRepresentation {
}

extension NSPasteboard.PasteboardType: Swift.RawRepresentable {
    typealias RawValue = Swift.String
}

extension NSPasteboard.PasteboardType: Swift._SwiftNewtypeWrapper {
}

extension NSPasteboard.PasteboardType: Swift.Equatable {
}

extension NSPasteboard.PasteboardType: Swift.Hashable {
}

extension NSPasteboard.PasteboardType: Swift._ObjectiveCBridgeable {
    typealias _ObjectiveCType = NSString
}
