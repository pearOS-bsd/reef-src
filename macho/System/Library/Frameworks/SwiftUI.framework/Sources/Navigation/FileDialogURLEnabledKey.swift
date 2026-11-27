// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FileDialogURLEnabledKey (Navigation).

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

enum FileDialogURLEnabledKey {
}

extension SwiftUI.FileDialogURLEnabledKey: SwiftUI.EnvironmentKey {
    typealias Value = (Foundation.Predicate<(Foundation.URL)>)?
    static var defaultValue: Self.Value { nil }
}
