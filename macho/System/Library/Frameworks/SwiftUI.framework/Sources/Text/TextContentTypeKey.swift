// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextContentTypeKey (Text).

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

enum TextContentTypeKey {
}

extension SwiftUI.TextContentTypeKey: SwiftUI.EnvironmentKey {
    typealias Value = Swift.String?
    static var defaultValue: Self.Value { _skeleton() }
}
