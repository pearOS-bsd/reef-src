// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToggleStyle (Drawing).

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

public protocol ToggleStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.ToggleStyleConfiguration) -> Self.Body
}

extension SwiftUI.ToggleStyle where Self == SwiftUI.ButtonToggleStyle {
    public static var button: SwiftUI.ButtonToggleStyle { _skeleton() }
}

extension SwiftUI.ToggleStyle where Self == SwiftUI.SwitchToggleStyle {
    public static var `switch`: SwiftUI.SwitchToggleStyle { _skeleton() }
}

extension SwiftUI.ToggleStyle where Self == SwiftUI.DefaultToggleStyle {
    public static var automatic: SwiftUI.DefaultToggleStyle { _skeleton() }
}

extension SwiftUI.ToggleStyle where Self == SwiftUI.CheckboxToggleStyle {
    public static var checkbox: SwiftUI.CheckboxToggleStyle { _skeleton() }
}
