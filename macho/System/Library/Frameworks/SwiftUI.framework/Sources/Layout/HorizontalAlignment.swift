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
    public static var center: SwiftUI.HorizontalAlignment { _skeleton() }
    public init(_ a0: any SwiftUI.AlignmentID.Type) { _skeleton() }
    public static var leading: SwiftUI.HorizontalAlignment { _skeleton() }
    public static var trailing: SwiftUI.HorizontalAlignment { _skeleton() }
    public static var listRowSeparatorLeading: SwiftUI.HorizontalAlignment { _skeleton() }
    public static var listRowSeparatorTrailing: SwiftUI.HorizontalAlignment { _skeleton() }
    public static func == (_ a0: SwiftUI.HorizontalAlignment, _ a1: SwiftUI.HorizontalAlignment) -> Swift.Bool { _skeleton() }
    enum ListRowSeparatorLeading {
    }
    enum ListRowSeparatorTrailing {
    }
}

extension SwiftUI.HorizontalAlignment: Swift.Equatable {
}

extension SwiftUI.HorizontalAlignment.ListRowSeparatorLeading: SwiftUI.AlignmentID {
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat { _skeleton() }
}

extension SwiftUI.HorizontalAlignment.ListRowSeparatorTrailing: SwiftUI.AlignmentID {
    static func defaultValue(`in` a0: SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat { _skeleton() }
}
