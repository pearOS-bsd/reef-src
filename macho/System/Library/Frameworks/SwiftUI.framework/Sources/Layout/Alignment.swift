// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Alignment (Layout).

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

public struct Alignment {
    public var horizontal: SwiftUI.HorizontalAlignment
    public var vertical: SwiftUI.VerticalAlignment
    public static var center: SwiftUI.Alignment { _skeleton() }
    public static var leading: SwiftUI.Alignment { _skeleton() }
    public static var trailing: SwiftUI.Alignment { _skeleton() }
    public static var top: SwiftUI.Alignment { _skeleton() }
    public static var bottom: SwiftUI.Alignment { _skeleton() }
    public static var topLeading: SwiftUI.Alignment { _skeleton() }
    public static var topTrailing: SwiftUI.Alignment { _skeleton() }
    public static var bottomLeading: SwiftUI.Alignment { _skeleton() }
    public static var bottomTrailing: SwiftUI.Alignment { _skeleton() }
    public static func == (_ a0: SwiftUI.Alignment, _ a1: SwiftUI.Alignment) -> Swift.Bool { _skeleton() }
    public static var centerFirstTextBaseline: SwiftUI.Alignment { _skeleton() }
    public static var centerLastTextBaseline: SwiftUI.Alignment { _skeleton() }
    public static var leadingFirstTextBaseline: SwiftUI.Alignment { _skeleton() }
    public static var leadingLastTextBaseline: SwiftUI.Alignment { _skeleton() }
    public static var trailingFirstTextBaseline: SwiftUI.Alignment { _skeleton() }
    public static var trailingLastTextBaseline: SwiftUI.Alignment { _skeleton() }
    public init(horizontal a0: SwiftUI.HorizontalAlignment, vertical a1: SwiftUI.VerticalAlignment) { _skeleton() }
}

extension SwiftUI.Alignment: Swift.Equatable {
}
