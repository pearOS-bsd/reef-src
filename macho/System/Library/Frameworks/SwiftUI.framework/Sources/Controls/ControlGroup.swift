// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlGroup (Controls).

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

public struct ControlGroup<A> where A: SwiftUI.View {
    let content: A
    public init<A1, B1>(content a0: () -> A1, label a1: () -> B1) where A == SwiftUI.LabeledControlGroupContent<A1, B1>, A1: SwiftUI.View, B1: SwiftUI.View { _skeleton() }
    public init<A1>(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> A1) where A == SwiftUI.LabeledControlGroupContent<A1, SwiftUI.Text>, A1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: B1, content a1: () -> A1) where A == SwiftUI.LabeledControlGroupContent<A1, SwiftUI.Text>, A1: SwiftUI.View, B1: Swift.StringProtocol { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    public init(content a0: () -> A) { _skeleton() }
}

extension SwiftUI.ControlGroup where A == SwiftUI.ControlGroupStyleConfiguration.Content {
    public init(_ a0: SwiftUI.ControlGroupStyleConfiguration) { _skeleton() }
}

extension SwiftUI.ControlGroup: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.ControlGroup: SwiftUI.PrimitiveView {
}
