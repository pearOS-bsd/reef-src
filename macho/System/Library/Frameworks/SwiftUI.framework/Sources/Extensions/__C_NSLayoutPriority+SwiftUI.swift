// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to __C_NSLayoutPriority.

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

extension NSLayoutConstraint.Priority: Swift._HasCustomAnyHashableRepresentation {
}

extension NSLayoutConstraint.Priority: Swift.RawRepresentable {
    typealias RawValue = Swift.Float
}

extension NSLayoutConstraint.Priority: Swift._SwiftNewtypeWrapper {
}

extension NSLayoutConstraint.Priority: Swift.Equatable {
}

extension NSLayoutConstraint.Priority: Swift.Hashable {
}

extension NSLayoutConstraint.Priority: Swift._ObjectiveCBridgeable {
    public typealias _ObjectiveCType = NSNumber
}
