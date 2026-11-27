// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MenuStyle (Drawing).

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

public protocol MenuStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.MenuStyleConfiguration) -> Self.Body
}

extension SwiftUI.MenuStyle where Self == SwiftUI.BorderlessButtonMenuStyle {
    public static var borderlessButton: SwiftUI.BorderlessButtonMenuStyle { _skeleton() }
}

extension SwiftUI.MenuStyle where Self == SwiftUI.ButtonMenuStyle {
    public static var button: SwiftUI.ButtonMenuStyle { _skeleton() }
}

extension SwiftUI.MenuStyle where Self == SwiftUI.DefaultMenuStyle {
    public static var automatic: SwiftUI.DefaultMenuStyle { _skeleton() }
}
