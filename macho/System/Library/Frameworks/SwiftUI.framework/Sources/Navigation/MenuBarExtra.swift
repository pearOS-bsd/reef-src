// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// MenuBarExtra (Navigation).

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

public struct MenuBarExtra<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let label: A
    let content: B
    let isInserted: (SwiftUI.Binding<Swift.Bool>)?
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    init(label a0: A, content a1: B, isInserted a2: (SwiftUI.Binding<Swift.Bool>)?) { _skeleton() }
    public init(isInserted a0: SwiftUI.Binding<Swift.Bool>, content a1: () -> B, label a2: () -> A) { _skeleton() }
    public init(content a0: () -> B, label a1: () -> A) { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.MenuBarExtra where A == SwiftUI.Text {
    public init(_ a0: SwiftUI.LocalizedStringKey, isInserted a1: SwiftUI.Binding<Swift.Bool>, content a2: () -> B) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, isInserted a1: SwiftUI.Binding<Swift.Bool>, content a2: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> B) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.MenuBarExtra where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image> {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, isInserted a2: SwiftUI.Binding<Swift.Bool>, content a3: () -> B) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, content a2: () -> B) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, content a2: () -> B) where A1: Swift.StringProtocol { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, image a1: Swift.String, content a2: () -> B) { _skeleton() }
}

extension SwiftUI.MenuBarExtra: SwiftUI.PrimitiveScene {
}

extension SwiftUI.MenuBarExtra: SwiftUI.Scene {
    public typealias Body = Swift.Never
}
