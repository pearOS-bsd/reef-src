// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FormStyle (Drawing).

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

public protocol FormStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.FormStyleConfiguration) -> Self.Body
}

extension SwiftUI.FormStyle where Self == SwiftUI.GroupedFormStyle {
    public static var grouped: SwiftUI.GroupedFormStyle { _skeleton() }
}

extension SwiftUI.FormStyle where Self == SwiftUI.ColumnsFormStyle {
    public static var columns: SwiftUI.ColumnsFormStyle { _skeleton() }
}

extension SwiftUI.FormStyle where Self == SwiftUI.AutomaticFormStyle {
    public static var automatic: SwiftUI.AutomaticFormStyle { _skeleton() }
}
