// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DisclosureGroupStyle (Drawing).

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

public protocol DisclosureGroupStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.DisclosureGroupStyleConfiguration) -> Self.Body
}

extension SwiftUI.DisclosureGroupStyle where Self == SwiftUI.AutomaticDisclosureGroupStyle {
    public static var automatic: SwiftUI.AutomaticDisclosureGroupStyle { _skeleton() }
}
