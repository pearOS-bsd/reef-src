// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PrimitiveButtonStyle (Drawing).

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

public protocol PrimitiveButtonStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.PrimitiveButtonStyleConfiguration) -> Self.Body
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.GlassButtonStyle {
    public static var glass: SwiftUI.GlassButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.GlassProminentButtonStyle {
    public static var glassProminent: SwiftUI.GlassProminentButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.BorderlessButtonStyle {
    public static var borderless: SwiftUI.BorderlessButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.AccessoryBarButtonStyle {
    public static var accessoryBar: SwiftUI.AccessoryBarButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.BorderedProminentButtonStyle {
    public static var borderedProminent: SwiftUI.BorderedProminentButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.AccessoryBarActionButtonStyle {
    public static var accessoryBarAction: SwiftUI.AccessoryBarActionButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.LinkButtonStyle {
    public static var link: SwiftUI.LinkButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.PlainButtonStyle {
    public static var plain: SwiftUI.PlainButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.DefaultButtonStyle {
    public static var automatic: SwiftUI.DefaultButtonStyle { _skeleton() }
}

extension SwiftUI.PrimitiveButtonStyle where Self == SwiftUI.BorderedButtonStyle {
    public static var bordered: SwiftUI.BorderedButtonStyle { _skeleton() }
}
