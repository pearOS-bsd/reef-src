// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Layout (Layout).

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

public protocol Layout: SwiftUI.Animatable {
    associatedtype Cache
    static var layoutProperties: SwiftUI.LayoutProperties { get }
    func updateCache(_ a0: inout Self.Cache, subviews a1: SwiftUI.LayoutSubviews)
    func spacing(subviews a0: SwiftUI.LayoutSubviews, cache a1: inout Self.Cache) -> SwiftUI.ViewSpacing
    func explicitAlignment(of a0: SwiftUI.HorizontalAlignment, `in` a1: CGRect, proposal a2: SwiftUI.ProposedViewSize, subviews a3: SwiftUI.LayoutSubviews, cache a4: inout Self.Cache) -> CoreGraphics.CGFloat?
    static func _makeLayoutView(root a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewOutputs
    func makeCache(subviews a0: SwiftUI.LayoutSubviews) -> Self.Cache
    func sizeThatFits(proposal a0: SwiftUI.ProposedViewSize, subviews a1: SwiftUI.LayoutSubviews, cache a2: inout Self.Cache) -> CGSize
    func placeSubviews(`in` a0: CGRect, proposal a1: SwiftUI.ProposedViewSize, subviews a2: SwiftUI.LayoutSubviews, cache a3: inout Self.Cache)
}

extension SwiftUI.Layout {
    public static var layoutProperties: SwiftUI.LayoutProperties { _skeleton() }
    public func updateCache(_ a0: inout Self.Cache, subviews a1: SwiftUI.LayoutSubviews) { _skeleton() }
    public func spacing(subviews a0: SwiftUI.LayoutSubviews, cache a1: inout Self.Cache) -> SwiftUI.ViewSpacing { _skeleton() }
    public func explicitAlignment(of a0: SwiftUI.HorizontalAlignment, `in` a1: CGRect, proposal a2: SwiftUI.ProposedViewSize, subviews a3: SwiftUI.LayoutSubviews, cache a4: inout Self.Cache) -> CoreGraphics.CGFloat? { _skeleton() }
    public func explicitAlignment(of a0: SwiftUI.VerticalAlignment, `in` a1: CGRect, proposal a2: SwiftUI.ProposedViewSize, subviews a3: SwiftUI.LayoutSubviews, cache a4: inout Self.Cache) -> CoreGraphics.CGFloat? { _skeleton() }
    public static func _makeLayoutView(root a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public func callAsFunction<A1>(_ a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { _skeleton() }
}

extension SwiftUI.Layout where Self.Cache == () {
    public func makeCache(subviews a0: SwiftUI.LayoutSubviews) { _skeleton() }
}
