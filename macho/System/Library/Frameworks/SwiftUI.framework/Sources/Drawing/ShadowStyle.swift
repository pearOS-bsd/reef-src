// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShadowStyle (Drawing).

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

public struct ShadowStyle {
    let kind: SwiftUI.ShadowStyle.Kind
    let color: SwiftUI.Color
    let radius: CoreGraphics.CGFloat
    let x: CoreGraphics.CGFloat
    let y: CoreGraphics.CGFloat
    public static func drop(color a0: SwiftUI.Color, radius a1: CoreGraphics.CGFloat, x a2: CoreGraphics.CGFloat, y a3: CoreGraphics.CGFloat) -> SwiftUI.ShadowStyle { _skeleton() }
    public static func inner(color a0: SwiftUI.Color, radius a1: CoreGraphics.CGFloat, x a2: CoreGraphics.CGFloat, y a3: CoreGraphics.CGFloat) -> SwiftUI.ShadowStyle { _skeleton() }
    public static func == (_ a0: SwiftUI.ShadowStyle, _ a1: SwiftUI.ShadowStyle) -> Swift.Bool { _skeleton() }
    enum Kind {
        case drop
        case inner
    }
}

extension SwiftUI.ShadowStyle: Swift.Equatable {
}

extension SwiftUI.ShadowStyle.Kind: Swift.Equatable {
}

extension SwiftUI.ShadowStyle.Kind: Swift.Hashable {
}
