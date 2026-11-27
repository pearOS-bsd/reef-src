// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OnMoveTraitKey (Environment).

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

public struct OnMoveTraitKey {
    public static var defaultValue: ((Foundation.IndexSet, Swift.Int) -> ())? { _skeleton() }
}

extension SwiftUI.OnMoveTraitKey: SwiftUI._ViewTraitKey {
    public typealias Value = ((Foundation.IndexSet, Swift.Int) -> ())?
}
