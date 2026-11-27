// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MenuBarExtraStyle (Drawing).

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

public protocol MenuBarExtraStyle {
}

extension SwiftUI.MenuBarExtraStyle where Self == SwiftUI.AutomaticMenuBarExtraStyle {
    public static var automatic: SwiftUI.AutomaticMenuBarExtraStyle { _skeleton() }
}

extension SwiftUI.MenuBarExtraStyle where Self == SwiftUI.PullDownMenuBarExtraStyle {
    public static var menu: SwiftUI.PullDownMenuBarExtraStyle { _skeleton() }
}

extension SwiftUI.MenuBarExtraStyle where Self == SwiftUI.WindowMenuBarExtraStyle {
    public static var window: SwiftUI.WindowMenuBarExtraStyle { _skeleton() }
}
