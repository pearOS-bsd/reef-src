// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Window (Navigation).

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

public struct Window<A> where A: SwiftUI.View {
    let title: SwiftUI.Text
    let id: Swift.String
    let content: () -> A
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    public init(_ a0: SwiftUI.Text, id a1: Swift.String, content a2: () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, id a1: Swift.String, content a2: () -> A) { _skeleton() }
    public init<A1>(_ a0: A1, id a1: Swift.String, content a2: () -> A) where A1: Swift.StringProtocol { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.Window: SwiftUI.PrimitiveScene {
}

extension SwiftUI.Window: SwiftUI.Scene {
    public typealias Body = Swift.Never
}
