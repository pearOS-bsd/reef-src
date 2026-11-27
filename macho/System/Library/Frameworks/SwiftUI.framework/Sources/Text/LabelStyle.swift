// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabelStyle (Text).

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

public protocol LabelStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.LabelStyleConfiguration) -> Self.Body
}

extension SwiftUI.LabelStyle where Self == SwiftUI.TitleAndIconLabelStyle {
    public static var titleAndIcon: SwiftUI.TitleAndIconLabelStyle { _skeleton() }
}

extension SwiftUI.LabelStyle where Self == SwiftUI.DefaultLabelStyle {
    public static var automatic: SwiftUI.DefaultLabelStyle { _skeleton() }
}

extension SwiftUI.LabelStyle where Self == SwiftUI.IconOnlyLabelStyle {
    public static var iconOnly: SwiftUI.IconOnlyLabelStyle { _skeleton() }
}

extension SwiftUI.LabelStyle where Self == SwiftUI.TitleOnlyLabelStyle {
    public static var titleOnly: SwiftUI.TitleOnlyLabelStyle { _skeleton() }
}
