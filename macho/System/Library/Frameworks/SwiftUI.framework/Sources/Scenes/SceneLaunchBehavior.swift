// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SceneLaunchBehavior (Scenes).

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

public struct SceneLaunchBehavior {
    let rawValue: Swift.Int
    public static var presented: SwiftUI.SceneLaunchBehavior { SwiftUI.SceneLaunchBehavior(rawValue: 1) }
    public static var suppressed: SwiftUI.SceneLaunchBehavior { SwiftUI.SceneLaunchBehavior(rawValue: 2) }
    public static var automatic: SwiftUI.SceneLaunchBehavior { SwiftUI.SceneLaunchBehavior(rawValue: 0) }
    public static func == (_ a0: SwiftUI.SceneLaunchBehavior, _ a1: SwiftUI.SceneLaunchBehavior) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.SceneLaunchBehavior: Swift.Equatable {
}
