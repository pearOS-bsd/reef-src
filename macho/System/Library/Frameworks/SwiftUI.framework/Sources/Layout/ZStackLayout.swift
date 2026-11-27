// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ZStackLayout (Layout).

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

public struct ZStackLayout {
    public var alignment: SwiftUI.Alignment
    public func sizeThatFits(proposal a0: SwiftUI.ProposedViewSize, subviews a1: SwiftUI.LayoutSubviews, cache a2: inout ()) -> CGSize { _skeleton() }
    public func placeSubviews(`in` a0: CGRect, proposal a1: SwiftUI.ProposedViewSize, subviews a2: SwiftUI.LayoutSubviews, cache a3: inout ()) { _skeleton() }
    public func makeCache(subviews a0: SwiftUI.LayoutSubviews) { _skeleton() }
    public init(alignment a0: SwiftUI.Alignment) { _skeleton() }
}

extension SwiftUI.ZStackLayout: SwiftUI.Layout {
    public typealias Cache = ()
}

extension SwiftUI.ZStackLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}
