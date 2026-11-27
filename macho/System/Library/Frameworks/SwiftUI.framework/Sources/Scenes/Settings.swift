// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Settings (Scenes).

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

public struct Settings<A> where A: SwiftUI.View {
    let content: A
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    public init(content a0: () -> A) { _skeleton() }
}

extension SwiftUI.Settings: SwiftUI.PrimitiveScene {
}

extension SwiftUI.Settings: SwiftUI.Scene {
    public typealias Body = Swift.Never
}
