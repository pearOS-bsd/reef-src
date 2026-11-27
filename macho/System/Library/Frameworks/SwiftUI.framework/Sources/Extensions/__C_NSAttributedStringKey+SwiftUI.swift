// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to __C_NSAttributedStringKey.

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

extension NSAttributedString.Key: Swift._HasCustomAnyHashableRepresentation {
}

extension NSAttributedString.Key: Swift.RawRepresentable {
    typealias RawValue = Swift.String
}

extension NSAttributedString.Key: Swift._SwiftNewtypeWrapper {
}

extension NSAttributedString.Key: Swift.Equatable {
}

extension NSAttributedString.Key: Swift._ObjectiveCBridgeable {
    typealias _ObjectiveCType = NSString
}

extension NSAttributedString.Key: Swift.Hashable {
}
