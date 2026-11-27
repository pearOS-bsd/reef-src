// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Picker (Controls).

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

public struct Picker<A, B, C> where A: SwiftUI.View, B: Swift.Hashable, C: SwiftUI.View {
    let selection: SwiftUI.Binding<B>
    let label: A
    let content: C
    let currentValueLabel: SwiftUI.AnyView?
    init(selection a0: SwiftUI.Binding<B>, label a1: A, content a2: C, currentValueLabel a3: SwiftUI.AnyView?) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<B>, label a1: A, content a2: () -> C) { _skeleton() }
    public init(selection a0: SwiftUI.Binding<B>, content a1: () -> C, label a2: () -> A) { _skeleton() }
    public init<A1>(selection a0: SwiftUI.Binding<B>, content a1: () -> C, label a2: () -> A, currentValueLabel a3: () -> A1) where A1: SwiftUI.View { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func buildPicker(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}

extension SwiftUI.Picker where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, selection a1: SwiftUI.Binding<B>, content a2: () -> C) { _skeleton() }
    public init<A1>(_ a0: A1, selection a1: SwiftUI.Binding<B>, content a2: () -> C) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Picker where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, selection a2: SwiftUI.Binding<B>, content a3: () -> C) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, selection a2: SwiftUI.Binding<B>, content a3: () -> C) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.Picker: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Picker: SwiftUI.PrimitiveView {
}

extension SwiftUI.Picker: SwiftUI.MenuContributor {
}
