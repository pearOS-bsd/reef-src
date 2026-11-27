// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GroupBoxStyle (Drawing).

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

public protocol GroupBoxStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.GroupBoxStyleConfiguration) -> Self.Body
}

extension SwiftUI.GroupBoxStyle where Self == SwiftUI.DefaultGroupBoxStyle {
    public static var automatic: SwiftUI.DefaultGroupBoxStyle { _skeleton() }
}
