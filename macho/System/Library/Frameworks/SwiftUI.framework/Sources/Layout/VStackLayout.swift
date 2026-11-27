// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VStackLayout (Layout).

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

public struct VStackLayout {
    public var alignment: SwiftUI.HorizontalAlignment
    public var spacing: CoreGraphics.CGFloat?
    public func sizeThatFits(proposal a0: SwiftUI.ProposedViewSize, subviews a1: SwiftUI.LayoutSubviews, cache a2: inout ()) -> CGSize { _skeleton() }
    public func placeSubviews(`in` a0: CGRect, proposal a1: SwiftUI.ProposedViewSize, subviews a2: SwiftUI.LayoutSubviews, cache a3: inout ()) { _skeleton() }
    public func makeCache(subviews a0: SwiftUI.LayoutSubviews) { _skeleton() }
    public init(alignment a0: SwiftUI.HorizontalAlignment, spacing a1: CoreGraphics.CGFloat?) { _skeleton() }
}

extension SwiftUI.VStackLayout: SwiftUI.AnyLayoutRoot {
    func element(_ a0: [SwiftUI.Element]) -> SwiftUI.Element { _skeleton() }
}

extension SwiftUI.VStackLayout: SwiftUI.Layout {
    public typealias Cache = ()
}

extension SwiftUI.VStackLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}
