// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WindowResizability (Navigation).

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

public struct WindowResizability {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.WindowResizability { SwiftUI.WindowResizability(rawValue: 0) }
    public static var contentSize: SwiftUI.WindowResizability { SwiftUI.WindowResizability(rawValue: 1) }
    public static var contentMinSize: SwiftUI.WindowResizability { SwiftUI.WindowResizability(rawValue: 2) }
    public static func == (_ a0: SwiftUI.WindowResizability, _ a1: SwiftUI.WindowResizability) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.WindowResizability: Swift.Equatable {
}
