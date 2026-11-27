// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SceneRestorationBehavior (Scenes).

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

public struct SceneRestorationBehavior {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.SceneRestorationBehavior { SwiftUI.SceneRestorationBehavior(rawValue: 0) }
    public static var disabled: SwiftUI.SceneRestorationBehavior { SwiftUI.SceneRestorationBehavior(rawValue: 1) }
    public static func == (_ a0: SwiftUI.SceneRestorationBehavior, _ a1: SwiftUI.SceneRestorationBehavior) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.SceneRestorationBehavior: Swift.Equatable {
}
