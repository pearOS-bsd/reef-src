// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _LayoutTrait (Environment).

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

public struct _LayoutTrait<A> where A: SwiftUI.LayoutValueKey {
    public static var defaultValue: A.Value { _skeleton() }
}

extension SwiftUI._LayoutTrait: SwiftUI._ViewTraitKey {
    public typealias Value = A.Value
}
