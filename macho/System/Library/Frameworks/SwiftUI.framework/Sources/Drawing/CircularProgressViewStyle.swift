// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CircularProgressViewStyle (Drawing).

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

public struct CircularProgressViewStyle {
    var tint: SwiftUI.Color?
    public func makeBody(configuration a0: SwiftUI.ProgressViewStyleConfiguration) -> SwiftUI.EmptyView { _skeleton() }
    public init() { _skeleton() }
    public init(tint a0: SwiftUI.Color) { _skeleton() }
}

extension SwiftUI.CircularProgressViewStyle: SwiftUI.ProgressViewStyle {
    public typealias Body = SwiftUI.EmptyView
}
