// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlGroupStyle (Drawing).

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

public protocol ControlGroupStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.ControlGroupStyleConfiguration) -> Self.Body
}

extension SwiftUI.ControlGroupStyle where Self == SwiftUI.NavigationControlGroupStyle {
    public static var navigation: SwiftUI.NavigationControlGroupStyle { _skeleton() }
}

extension SwiftUI.ControlGroupStyle where Self == SwiftUI.CompactMenuControlGroupStyle {
    public static var compactMenu: SwiftUI.CompactMenuControlGroupStyle { _skeleton() }
}

extension SwiftUI.ControlGroupStyle where Self == SwiftUI.MenuControlGroupStyle {
    public static var menu: SwiftUI.MenuControlGroupStyle { _skeleton() }
}

extension SwiftUI.ControlGroupStyle where Self == SwiftUI.PaletteControlGroupStyle {
    public static var palette: SwiftUI.PaletteControlGroupStyle { _skeleton() }
}

extension SwiftUI.ControlGroupStyle where Self == SwiftUI.AutomaticControlGroupStyle {
    public static var automatic: SwiftUI.AutomaticControlGroupStyle { _skeleton() }
}
