// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _OptionalScene (Scenes).

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

struct _OptionalScene {
    let content: (any SwiftUI.Scene)?
}

extension SwiftUI._OptionalScene: SwiftUI.Scene {
    typealias Body = Swift.Never
    var body: Self.Body { _skeleton() }
}

extension SwiftUI._OptionalScene: SwiftUI.PrimitiveScene {
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
}
