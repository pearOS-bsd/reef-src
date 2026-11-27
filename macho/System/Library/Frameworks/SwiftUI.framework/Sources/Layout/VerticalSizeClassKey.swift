// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VerticalSizeClassKey (Layout).

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

enum VerticalSizeClassKey {
}

extension SwiftUI.VerticalSizeClassKey: SwiftUI.EnvironmentKey {
    typealias Value = SwiftUI.UserInterfaceSizeClass?
    static var defaultValue: Self.Value { .regular }
}
