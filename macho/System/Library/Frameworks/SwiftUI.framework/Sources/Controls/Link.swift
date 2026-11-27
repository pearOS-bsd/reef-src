// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Link (Controls).

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

public struct Link<A> where A: SwiftUI.View {
    let destination: Foundation.URL
    let label: A
    public init(destination a0: Foundation.URL, label a1: () -> A) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.Link where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, destination a1: Foundation.URL) { _skeleton() }
    public init<A1>(_ a0: A1, destination a1: Foundation.URL) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Link: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Link: SwiftUI.PrimitiveView {
}
