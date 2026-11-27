// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FileDialogDefaultDirectoryKey (Navigation).

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

enum FileDialogDefaultDirectoryKey {
    static var defaultValue: Foundation.URL? { _skeleton() }
}

extension SwiftUI.FileDialogDefaultDirectoryKey: SwiftUI.EnvironmentKey {
    typealias Value = Foundation.URL?
}
