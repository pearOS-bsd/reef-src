// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProposedViewSize (Layout).

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

public struct ProposedViewSize {
    public var width: CoreGraphics.CGFloat?
    public var height: CoreGraphics.CGFloat?
    public func replacingUnspecifiedDimensions(by a0: CGSize) -> CGSize { CGSize(width: width ?? a0.width, height: height ?? a0.height) }
    public init(width a0: CoreGraphics.CGFloat?, height a1: CoreGraphics.CGFloat?) {
        width = a0
        height = a1
    }
    public init(_ a0: CGSize) {
        width = a0.width
        height = a0.height
    }
    public static var zero: SwiftUI.ProposedViewSize { SwiftUI.ProposedViewSize(width: 0, height: 0) }
    public static var unspecified: SwiftUI.ProposedViewSize { SwiftUI.ProposedViewSize(width: nil, height: nil) }
    public static var infinity: SwiftUI.ProposedViewSize { SwiftUI.ProposedViewSize(width: .infinity, height: .infinity) }
    public static func == (_ a0: SwiftUI.ProposedViewSize, _ a1: SwiftUI.ProposedViewSize) -> Swift.Bool { a0.width == a1.width && a0.height == a1.height }
}

extension SwiftUI.ProposedViewSize: Swift.Equatable {
}
