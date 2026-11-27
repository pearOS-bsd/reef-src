// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentMarginPlacement (Views).

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

public struct ContentMarginPlacement {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ContentMarginPlacement { SwiftUI.ContentMarginPlacement(rawValue: 0) }
    public static var scrollContent: SwiftUI.ContentMarginPlacement { _skeleton() }
    public static var scrollIndicators: SwiftUI.ContentMarginPlacement { SwiftUI.ContentMarginPlacement(rawValue: 2) }
}
