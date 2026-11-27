// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabViewStyle (Drawing).

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

public protocol TabViewStyle {
}

extension SwiftUI.TabViewStyle where Self == SwiftUI.SidebarAdaptableTabViewStyle {
    public static var sidebarAdaptable: SwiftUI.SidebarAdaptableTabViewStyle { _skeleton() }
}

extension SwiftUI.TabViewStyle where Self == SwiftUI.DefaultTabViewStyle {
    public static var automatic: SwiftUI.DefaultTabViewStyle { _skeleton() }
}

extension SwiftUI.TabViewStyle where Self == SwiftUI.TabBarOnlyTabViewStyle {
    public static var tabBarOnly: SwiftUI.TabBarOnlyTabViewStyle { _skeleton() }
}
