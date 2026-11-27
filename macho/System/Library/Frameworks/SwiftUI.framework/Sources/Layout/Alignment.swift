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
    public static let center = SwiftUI.Alignment(horizontal: .center, vertical: .center)
    public static let leading = SwiftUI.Alignment(horizontal: .leading, vertical: .center)
    public static let trailing = SwiftUI.Alignment(horizontal: .trailing, vertical: .center)
    public static let top = SwiftUI.Alignment(horizontal: .center, vertical: .top)
    public static let bottom = SwiftUI.Alignment(horizontal: .center, vertical: .bottom)
    public static let topLeading = SwiftUI.Alignment(horizontal: .leading, vertical: .top)
    public static let topTrailing = SwiftUI.Alignment(horizontal: .trailing, vertical: .top)
    public static let bottomLeading = SwiftUI.Alignment(horizontal: .leading, vertical: .bottom)
    public static let bottomTrailing = SwiftUI.Alignment(horizontal: .trailing, vertical: .bottom)
    public static func == (_ a0: SwiftUI.Alignment, _ a1: SwiftUI.Alignment) -> Swift.Bool { a0.horizontal == a1.horizontal && a0.vertical == a1.vertical }
    public static let centerFirstTextBaseline = SwiftUI.Alignment(horizontal: .center, vertical: .firstTextBaseline)
    public static let centerLastTextBaseline = SwiftUI.Alignment(horizontal: .center, vertical: .lastTextBaseline)
    public static let leadingFirstTextBaseline = SwiftUI.Alignment(horizontal: .leading, vertical: .firstTextBaseline)
    public static let leadingLastTextBaseline = SwiftUI.Alignment(horizontal: .leading, vertical: .lastTextBaseline)
    public static let trailingFirstTextBaseline = SwiftUI.Alignment(horizontal: .trailing, vertical: .firstTextBaseline)
    public static let trailingLastTextBaseline = SwiftUI.Alignment(horizontal: .trailing, vertical: .lastTextBaseline)
    public init(horizontal a0: SwiftUI.HorizontalAlignment, vertical a1: SwiftUI.VerticalAlignment) {
        horizontal = a0
        vertical = a1
    }
}

extension SwiftUI.Alignment: Swift.Equatable {
}
