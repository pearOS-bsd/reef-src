// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabItemTraitKey (Environment).

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

struct TabItemTraitKey {
    struct Value {
        var label: SwiftUI.AnyView?
    }
}

extension SwiftUI.TabItemTraitKey: SwiftUI._ViewTraitKey {
    static var defaultValue: Self.Value { Value(label: nil) }
}
