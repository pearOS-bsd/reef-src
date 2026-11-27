// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LayoutSubview (Layout).

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

public struct LayoutSubview {
    let storage: SwiftUI.LayoutChildren?
    let index: Swift.Int
    public func sizeThatFits(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    public func place(at a0: CGPoint, anchor a1: SwiftUI.UnitPoint, proposal a2: SwiftUI.ProposedViewSize) { _skeleton() }
    var element: SwiftUI.Element? { _skeleton() }
    public static func == (_ a0: SwiftUI.LayoutSubview, _ a1: SwiftUI.LayoutSubview) -> Swift.Bool { _skeleton() }
    public func dimensions(`in` a0: SwiftUI.ProposedViewSize) -> SwiftUI.ViewDimensions { _skeleton() }
    public subscript<A>(_ a0: A.Type) -> A.Value where A: SwiftUI.LayoutValueKey { _skeleton() }
    public var priority: Swift.Double { _skeleton() }
    public var spacing: SwiftUI.ViewSpacing { _skeleton() }
}

extension SwiftUI.LayoutSubview: Swift.Equatable {
}
