// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyGridElement (Layout).

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

class LazyGridElement: SwiftUI.Element {
    let axis: SwiftUI.Axis = _skeletonValue()
    let tracks: [SwiftUI.GridItem] = _skeletonValue()
    let cells: [SwiftUI.Element] = _skeletonValue()
    let spacing: CoreGraphics.CGFloat = _skeletonValue()
    let alignment: SwiftUI.AlignmentKey = _skeletonValue()
    final func arrange(_ a0: CoreGraphics.CGFloat?, place a1: CGPoint?) -> CGSize { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    init(axis a0: SwiftUI.Axis, tracks a1: [SwiftUI.GridItem], cells a2: [SwiftUI.Element], spacing a3: CoreGraphics.CGFloat?, alignment a4: SwiftUI.AlignmentKey) { _skeleton() }
    deinit {}
}
