// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LabeledContentStyle (Text).

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

public protocol LabeledContentStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.LabeledContentStyleConfiguration) -> Self.Body
}

extension SwiftUI.LabeledContentStyle where Self == SwiftUI.AutomaticLabeledContentStyle {
    public static var automatic: SwiftUI.AutomaticLabeledContentStyle { _skeleton() }
}
