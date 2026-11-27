// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragDropPreviewsFormation (Gestures).

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

public struct DragDropPreviewsFormation {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.DragDropPreviewsFormation { SwiftUI.DragDropPreviewsFormation(rawValue: 0) }
    public static var list: SwiftUI.DragDropPreviewsFormation { SwiftUI.DragDropPreviewsFormation(rawValue: 1) }
    public static var pile: SwiftUI.DragDropPreviewsFormation { SwiftUI.DragDropPreviewsFormation(rawValue: 2) }
    public static var stack: SwiftUI.DragDropPreviewsFormation { SwiftUI.DragDropPreviewsFormation(rawValue: 3) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.DragDropPreviewsFormation, _ a1: SwiftUI.DragDropPreviewsFormation) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.DragDropPreviewsFormation: Swift.Hashable {
}

extension SwiftUI.DragDropPreviewsFormation: Swift.Equatable {
}
