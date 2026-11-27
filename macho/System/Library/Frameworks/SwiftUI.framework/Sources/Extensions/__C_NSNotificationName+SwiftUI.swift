// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to __C_NSNotificationName.

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

extension NSNotification.Name {
    static var textFieldDidChangeSelection: NSNotification.Name { _skeleton() }
}

extension NSNotification.Name: Swift._HasCustomAnyHashableRepresentation {
}

extension NSNotification.Name: Swift.RawRepresentable {
    typealias RawValue = Swift.String
}

extension NSNotification.Name: Swift._SwiftNewtypeWrapper {
}

extension NSNotification.Name: Swift.Equatable {
}

extension NSNotification.Name: Swift.Hashable {
}

extension NSNotification.Name: Swift._ObjectiveCBridgeable {
    typealias _ObjectiveCType = NSString
}
