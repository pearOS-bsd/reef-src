// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GridItem (Layout).

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

public struct GridItem {
    public var size: SwiftUI.GridItem.Size
    public var spacing: CoreGraphics.CGFloat?
    public var alignment: SwiftUI.Alignment?
    public init(_ a0: SwiftUI.GridItem.Size, spacing a1: CoreGraphics.CGFloat?, alignment a2: SwiftUI.Alignment?) { _skeleton() }
    public enum Size {
        case fixed(CoreGraphics.CGFloat)
        case flexible(minimum: CoreGraphics.CGFloat, maximum: CoreGraphics.CGFloat)
        case adaptive(minimum: CoreGraphics.CGFloat, maximum: CoreGraphics.CGFloat)
    }
}
