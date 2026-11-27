// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WindowToolbarStyle (Drawing).

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

public protocol WindowToolbarStyle {
}

extension SwiftUI.WindowToolbarStyle where Self == SwiftUI.DefaultWindowToolbarStyle {
    public static var automatic: SwiftUI.DefaultWindowToolbarStyle { _skeleton() }
}

extension SwiftUI.WindowToolbarStyle where Self == SwiftUI.UnifiedWindowToolbarStyle {
    public static var unified: SwiftUI.UnifiedWindowToolbarStyle { _skeleton() }
    public static func unified(showsTitle a0: Swift.Bool) -> SwiftUI.UnifiedWindowToolbarStyle { _skeleton() }
}

extension SwiftUI.WindowToolbarStyle where Self == SwiftUI.UnifiedCompactWindowToolbarStyle {
    public static var unifiedCompact: SwiftUI.UnifiedCompactWindowToolbarStyle { _skeleton() }
}

extension SwiftUI.WindowToolbarStyle where Self == SwiftUI.ExpandedWindowToolbarStyle {
    public static var expanded: SwiftUI.ExpandedWindowToolbarStyle { _skeleton() }
}
