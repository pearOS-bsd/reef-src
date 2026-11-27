// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreviewLayoutTraitKey (Environment).

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

public struct PreviewLayoutTraitKey {
    public static var defaultValue: SwiftUI.PreviewLayout { _skeleton() }
}

extension SwiftUI.PreviewLayoutTraitKey: SwiftUI._ViewTraitKey {
    public typealias Value = SwiftUI.PreviewLayout
}
