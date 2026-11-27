// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SceneOptions (Scenes).

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

struct SceneOptions {
    var defaultSize: CGSize?
    var resizability: SwiftUI.WindowResizability
    var hidesTitleBar: Swift.Bool
    var commands: [any SwiftUI.Commands]
    var removesCommands: Swift.Bool
    var environment: [(inout SwiftUI.EnvironmentValues) -> ()]
    var launchBehavior: SwiftUI.SceneLaunchBehavior
    var backgroundDrag: SwiftUI.WindowInteractionBehavior
}
