// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _LimitedAvailabilityScene (Scenes).

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

struct _LimitedAvailabilityScene<A> where A: SwiftUI.Scene {
    let content: A
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    init(content a0: A) { _skeleton() }
}

extension SwiftUI._LimitedAvailabilityScene: SwiftUI._LimitedAvailabilitySceneMarker {
}

extension SwiftUI._LimitedAvailabilityScene: SwiftUI.Scene {
    typealias Body = Swift.Never
    var body: Self.Body { _skeleton() }
}

extension SwiftUI._LimitedAvailabilityScene: SwiftUI.PrimitiveScene {
}
