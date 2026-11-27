// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FileDialogBrowserOptionsKey (Navigation).

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

struct FileDialogBrowserOptionsKey {
}

extension SwiftUI.FileDialogBrowserOptionsKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.FileDialogBrowserOptions
    static var defaultValue: Self.Value { _skeleton() }
}
