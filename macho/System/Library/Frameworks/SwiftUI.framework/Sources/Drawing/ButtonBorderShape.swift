// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ButtonBorderShape (Drawing).

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

public struct ButtonBorderShape {
    let kind: SwiftUI.ButtonBorderShape.Kind
    public static var automatic: SwiftUI.ButtonBorderShape { _skeleton() }
    public static var capsule: SwiftUI.ButtonBorderShape { _skeleton() }
    public static var roundedRectangle: SwiftUI.ButtonBorderShape { _skeleton() }
    public static var circle: SwiftUI.ButtonBorderShape { _skeleton() }
    public static func roundedRectangle(radius a0: CoreGraphics.CGFloat) -> SwiftUI.ButtonBorderShape { _skeleton() }
    public static func == (_ a0: SwiftUI.ButtonBorderShape, _ a1: SwiftUI.ButtonBorderShape) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    enum Kind {
        case roundedRectangle(CoreGraphics.CGFloat?)
        case automatic
        case capsule
        case circle
        var hashValue: Swift.Int { _skeleton() }
    }
}

extension SwiftUI.ButtonBorderShape: Swift.Equatable {
}

extension SwiftUI.ButtonBorderShape: Swift.Hashable {
}

extension SwiftUI.ButtonBorderShape.Kind: Swift.Equatable {
}

extension SwiftUI.ButtonBorderShape.Kind: Swift.Hashable {
}
