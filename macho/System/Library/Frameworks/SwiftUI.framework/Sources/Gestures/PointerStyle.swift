// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PointerStyle (Gestures).

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

public struct PointerStyle {
    let kind: SwiftUI.PointerStyle.Kind
    public static var `default`: SwiftUI.PointerStyle { _skeleton() }
    public static var grabIdle: SwiftUI.PointerStyle { _skeleton() }
    public static var grabActive: SwiftUI.PointerStyle { _skeleton() }
    public static var horizontalText: SwiftUI.PointerStyle { _skeleton() }
    public static var verticalText: SwiftUI.PointerStyle { _skeleton() }
    public static var link: SwiftUI.PointerStyle { _skeleton() }
    public static var rectSelection: SwiftUI.PointerStyle { _skeleton() }
    public static var columnResize: SwiftUI.PointerStyle { _skeleton() }
    public static var rowResize: SwiftUI.PointerStyle { _skeleton() }
    public static func columnResize(directions a0: SwiftUI.HorizontalDirection.Set) -> SwiftUI.PointerStyle { _skeleton() }
    public static func rowResize(directions a0: SwiftUI.VerticalDirection.Set) -> SwiftUI.PointerStyle { _skeleton() }
    public static func frameResize(position a0: SwiftUI.FrameResizePosition, directions a1: SwiftUI.FrameResizeDirection.Set) -> SwiftUI.PointerStyle { _skeleton() }
    var cursor: NSCursor { _skeleton() }
    public static func == (_ a0: SwiftUI.PointerStyle, _ a1: SwiftUI.PointerStyle) -> Swift.Bool { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    enum Kind {
        case frameResize(SwiftUI.FrameResizePosition, SwiftUI.FrameResizeDirection.Set)
        case columnResize(SwiftUI.HorizontalDirection.Set)
        case rowResize(SwiftUI.VerticalDirection.Set)
        case arrow
        case grabIdle
        case grabActive
        case horizontalText
        case verticalText
        case link
        case rectSelection
        func hash(into a0: inout Swift.Hasher) { _skeleton() }
    }
}

extension SwiftUI.PointerStyle: Swift.Hashable {
}

extension SwiftUI.PointerStyle: Swift.Equatable {
}

extension SwiftUI.PointerStyle.Kind: Swift.Equatable {
}

extension SwiftUI.PointerStyle.Kind: Swift.Hashable {
}
