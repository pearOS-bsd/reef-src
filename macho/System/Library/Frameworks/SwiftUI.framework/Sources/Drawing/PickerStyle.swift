// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PickerStyle (Drawing).

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

public protocol PickerStyle {
}

extension SwiftUI.PickerStyle where Self == SwiftUI.RadioGroupPickerStyle {
    public static var radioGroup: SwiftUI.RadioGroupPickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.MenuPickerStyle {
    public static var menu: SwiftUI.MenuPickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.TabsPickerStyle {
    public static var tabs: SwiftUI.TabsPickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.InlinePickerStyle {
    public static var inline: SwiftUI.InlinePickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.DefaultPickerStyle {
    public static var automatic: SwiftUI.DefaultPickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.PalettePickerStyle {
    public static var palette: SwiftUI.PalettePickerStyle { _skeleton() }
}

extension SwiftUI.PickerStyle where Self == SwiftUI.SegmentedPickerStyle {
    public static var segmented: SwiftUI.SegmentedPickerStyle { _skeleton() }
}
