// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextSelectability (Text).

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

public protocol TextSelectability {
    static var allowsSelection: Swift.Bool { get }
}

extension SwiftUI.TextSelectability where Self == SwiftUI.EnabledTextSelectability {
    public static var enabled: SwiftUI.EnabledTextSelectability { _skeleton() }
}

extension SwiftUI.TextSelectability where Self == SwiftUI.DisabledTextSelectability {
    public static var disabled: SwiftUI.DisabledTextSelectability { _skeleton() }
}
