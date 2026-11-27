// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationSplitViewStyle (Drawing).

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

public protocol NavigationSplitViewStyle {
}

extension SwiftUI.NavigationSplitViewStyle where Self == SwiftUI.AutomaticNavigationSplitViewStyle {
    public static var automatic: SwiftUI.AutomaticNavigationSplitViewStyle { _skeleton() }
}

extension SwiftUI.NavigationSplitViewStyle where Self == SwiftUI.BalancedNavigationSplitViewStyle {
    public static var balanced: SwiftUI.BalancedNavigationSplitViewStyle { _skeleton() }
}

extension SwiftUI.NavigationSplitViewStyle where Self == SwiftUI.ProminentDetailNavigationSplitViewStyle {
    public static var prominentDetail: SwiftUI.ProminentDetailNavigationSplitViewStyle { _skeleton() }
}
