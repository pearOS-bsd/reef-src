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
    public func replacingUnspecifiedDimensions(by a0: CGSize) -> CGSize { _skeleton() }
    public init(width a0: CoreGraphics.CGFloat?, height a1: CoreGraphics.CGFloat?) { _skeleton() }
    public init(_ a0: CGSize) { _skeleton() }
    public static var zero: SwiftUI.ProposedViewSize { _skeleton() }
    public static var unspecified: SwiftUI.ProposedViewSize { _skeleton() }
    public static var infinity: SwiftUI.ProposedViewSize { _skeleton() }
    public static func == (_ a0: SwiftUI.ProposedViewSize, _ a1: SwiftUI.ProposedViewSize) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ProposedViewSize: Swift.Equatable {
}
