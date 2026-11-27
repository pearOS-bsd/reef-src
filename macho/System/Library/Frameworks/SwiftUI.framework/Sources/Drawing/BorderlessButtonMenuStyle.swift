// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BorderlessButtonMenuStyle (Drawing).

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

public struct BorderlessButtonMenuStyle {
    let showsMenuIndicator: Swift.Bool
    public func makeBody(configuration a0: SwiftUI.MenuStyleConfiguration) -> SwiftUI.MenuStyleConfiguration.Label { _skeleton() }
    public init() { _skeleton() }
    public init(showsMenuIndicator a0: Swift.Bool) { _skeleton() }
}

extension SwiftUI.BorderlessButtonMenuStyle: SwiftUI.MenuStyle {
    public typealias Body = SwiftUI.MenuStyleConfiguration.Label
}
