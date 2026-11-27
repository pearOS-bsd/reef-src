// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WindowInteractionBehavior (Navigation).

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

public struct WindowInteractionBehavior {
    let rawValue: Swift.Int
    public static var enabled: SwiftUI.WindowInteractionBehavior { SwiftUI.WindowInteractionBehavior(rawValue: 1) }
    public static var disabled: SwiftUI.WindowInteractionBehavior { SwiftUI.WindowInteractionBehavior(rawValue: 2) }
    public static var automatic: SwiftUI.WindowInteractionBehavior { SwiftUI.WindowInteractionBehavior(rawValue: 0) }
    public static func == (_ a0: SwiftUI.WindowInteractionBehavior, _ a1: SwiftUI.WindowInteractionBehavior) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.WindowInteractionBehavior: Swift.Equatable {
}
