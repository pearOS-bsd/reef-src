// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CustomLayoutElement (Layout).

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

class CustomLayoutElement<A>: SwiftUI.Element where A: SwiftUI.Layout {
    let layout: A = _skeletonValue()
    let storage: SwiftUI.LayoutChildren = _skeletonValue()
    var cache: A.Cache? = _skeletonValue()
    init(layout a0: A, children a1: [SwiftUI.Element]) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    final var subviews: SwiftUI.LayoutSubviews { _skeleton() }
    final func withCache<A1>(_ a0: (inout A.Cache) -> A1) -> A1 { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    deinit {}
}
