// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SearchFieldPlacement (Navigation).

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

public struct SearchFieldPlacement {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.SearchFieldPlacement { SwiftUI.SearchFieldPlacement(rawValue: 0) }
    public static var toolbar: SwiftUI.SearchFieldPlacement { SwiftUI.SearchFieldPlacement(rawValue: 1) }
    public static var sidebar: SwiftUI.SearchFieldPlacement { SwiftUI.SearchFieldPlacement(rawValue: 2) }
}
