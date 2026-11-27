// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListStyle (Drawing).

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

public protocol ListStyle {
}

extension SwiftUI.ListStyle where Self == SwiftUI.InsetListStyle {
    public static var inset: SwiftUI.InsetListStyle { _skeleton() }
}

extension SwiftUI.ListStyle where Self == SwiftUI.PlainListStyle {
    public static var plain: SwiftUI.PlainListStyle { _skeleton() }
}

extension SwiftUI.ListStyle where Self == SwiftUI.DefaultListStyle {
    public static var automatic: SwiftUI.DefaultListStyle { _skeleton() }
}

extension SwiftUI.ListStyle where Self == SwiftUI.SidebarListStyle {
    public static var sidebar: SwiftUI.SidebarListStyle { _skeleton() }
}

extension SwiftUI.ListStyle where Self == SwiftUI.BorderedListStyle {
    public static var bordered: SwiftUI.BorderedListStyle { _skeleton() }
}
