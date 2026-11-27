// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListRowSeparatorTraitKey (Environment).

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

struct ListRowSeparatorTraitKey {
    struct Separator {
        let visibility: SwiftUI.Visibility
        let edges: SwiftUI.VerticalEdge.Set
    }
}

extension SwiftUI.ListRowSeparatorTraitKey: SwiftUI._ViewTraitKey {
    typealias Value = SwiftUI.ListRowSeparatorTraitKey.Separator?
    static var defaultValue: Self.Value { _skeleton() }
}
