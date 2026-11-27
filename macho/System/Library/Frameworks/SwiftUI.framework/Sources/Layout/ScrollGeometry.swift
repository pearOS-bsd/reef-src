// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollGeometry (Layout).

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

public struct ScrollGeometry {
    public var contentOffset: CGPoint
    public var contentSize: CGSize
    public var contentInsets: SwiftUI.EdgeInsets
    public var containerSize: CGSize
    public init(contentOffset a0: CGPoint, contentSize a1: CGSize, contentInsets a2: SwiftUI.EdgeInsets, containerSize a3: CGSize) { _skeleton() }
    public var visibleRect: CGRect { _skeleton() }
    public var bounds: CGRect { _skeleton() }
    public var debugDescription: Swift.String { _skeleton() }
    public static func == (_ a0: SwiftUI.ScrollGeometry, _ a1: SwiftUI.ScrollGeometry) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ScrollGeometry: Swift.Equatable {
}

extension SwiftUI.ScrollGeometry: Swift.CustomDebugStringConvertible {
}
