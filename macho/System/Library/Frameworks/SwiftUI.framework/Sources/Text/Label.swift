// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Label (Text).

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

public struct Label<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let title: A
    let icon: B
    public init(title a0: () -> A, icon a1: () -> B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func custom<A1>(_ a0: A1, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A1: SwiftUI.LabelStyle { _skeleton() }
    var iconPart: Any { _skeleton() }
    var titlePart: Any { _skeleton() }
}

extension SwiftUI.Label where A == SwiftUI.Text, B == SwiftUI.Image {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, image a1: Swift.String) { _skeleton() }
    public init<A1>(_ a0: A1, image a1: Swift.String) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Label where A == SwiftUI.LabelStyleConfiguration.Title, B == SwiftUI.LabelStyleConfiguration.Icon {
    public init(_ a0: SwiftUI.LabelStyleConfiguration) { _skeleton() }
}

extension SwiftUI.Label: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Label: SwiftUI.PrimitiveView {
}

extension SwiftUI.Label: SwiftUI.LabelParts {
}
