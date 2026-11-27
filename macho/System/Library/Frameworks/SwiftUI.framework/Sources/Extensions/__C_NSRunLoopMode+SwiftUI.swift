// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to __C_NSRunLoopMode.

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

extension RunLoop.Mode: Swift._HasCustomAnyHashableRepresentation {
}

extension RunLoop.Mode: Swift.RawRepresentable {
    typealias RawValue = Swift.String
}

extension RunLoop.Mode: Swift._SwiftNewtypeWrapper {
}

extension RunLoop.Mode: Swift.Equatable {
}

extension RunLoop.Mode: Swift.Hashable {
}

extension RunLoop.Mode: Swift._ObjectiveCBridgeable {
    typealias _ObjectiveCType = NSString
}
