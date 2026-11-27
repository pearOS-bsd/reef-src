// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VerticalAlignment (Layout).

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

public struct VerticalAlignment {
    public let key: SwiftUI.AlignmentKey
    public static let center = SwiftUI.VerticalAlignment(SwiftUI.CenterVerticalAlignment.self)
    public init(_ a0: any SwiftUI.AlignmentID.Type) { key = SwiftUI.AlignmentKey(id: a0, axis: .vertical) }
    public static let lastTextBaseline = SwiftUI.VerticalAlignment(SwiftUI.LastTextBaselineAlignment.self)
    public static let top = SwiftUI.VerticalAlignment(SwiftUI.TopAlignment.self)
    public static let bottom = SwiftUI.VerticalAlignment(SwiftUI.BottomAlignment.self)
    public static let firstTextBaseline = SwiftUI.VerticalAlignment(SwiftUI.FirstTextBaselineAlignment.self)
    public static func == (_ a0: SwiftUI.VerticalAlignment, _ a1: SwiftUI.VerticalAlignment) -> Swift.Bool { a0.key == a1.key }
}

extension SwiftUI.VerticalAlignment: Swift.Equatable {
}
