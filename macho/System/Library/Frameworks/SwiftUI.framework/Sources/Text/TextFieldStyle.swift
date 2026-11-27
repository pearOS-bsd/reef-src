// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextFieldStyle (Text).

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

public protocol TextFieldStyle {
    associatedtype _Body: SwiftUI.View
    func _body(configuration a0: SwiftUI.TextField<SwiftUI._TextFieldStyleLabel>) -> Self._Body
}

extension SwiftUI.TextFieldStyle where Self == SwiftUI.SquareBorderTextFieldStyle {
    public static var squareBorder: SwiftUI.SquareBorderTextFieldStyle { _skeleton() }
}

extension SwiftUI.TextFieldStyle where Self == SwiftUI.RoundedBorderTextFieldStyle {
    public static var roundedBorder: SwiftUI.RoundedBorderTextFieldStyle { _skeleton() }
}

extension SwiftUI.TextFieldStyle where Self == SwiftUI.PlainTextFieldStyle {
    public static var plain: SwiftUI.PlainTextFieldStyle { _skeleton() }
}

extension SwiftUI.TextFieldStyle where Self == SwiftUI.DefaultTextFieldStyle {
    public static var automatic: SwiftUI.DefaultTextFieldStyle { _skeleton() }
}
