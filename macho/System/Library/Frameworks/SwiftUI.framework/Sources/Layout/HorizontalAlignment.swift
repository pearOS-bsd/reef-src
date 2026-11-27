// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HorizontalAlignment (Layout).

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

public struct HorizontalAlignment {
    public let key: SwiftUI.AlignmentKey
    public static let center = SwiftUI.HorizontalAlignment(SwiftUI.CenterHorizontalAlignment.self)
    public init(_ a0: any SwiftUI.AlignmentID.Type) { key = SwiftUI.AlignmentKey(id: a0, axis: .horizontal) }
    public static let leading = SwiftUI.HorizontalAlignment(SwiftUI.LeadingAlignment.self)
    public static let trailing = SwiftUI.HorizontalAlignment(SwiftUI.TrailingAlignment.self)
    public static let listRowSeparatorLeading = SwiftUI.HorizontalAlignment(SwiftUI.HorizontalAlignment.ListRowSeparatorLeading.self)
    public static let listRowSeparatorTrailing = SwiftUI.HorizontalAlignment(SwiftUI.HorizontalAlignment.ListRowSeparatorTrailing.self)
    public static func == (_ a0: SwiftUI.HorizontalAlignment, _ a1: SwiftUI.HorizontalAlignment) -> Swift.Bool { a0.key == a1.key }
    enum ListRowSeparatorLeading {
    }
    enum ListRowSeparatorTrailing {
    }
}

extension SwiftUI.HorizontalAlignment: Swift.Equatable {
}

extension SwiftUI.HorizontalAlignment.ListRowSeparatorLeading: SwiftUI.AlignmentID {
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat { a0[SwiftUI.HorizontalAlignment.leading] }
}

extension SwiftUI.HorizontalAlignment.ListRowSeparatorTrailing: SwiftUI.AlignmentID {
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat { a0[SwiftUI.HorizontalAlignment.trailing] }
}
