// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SafeAreaInsetElement (Layout).

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

class SafeAreaInsetElement: SwiftUI.Element {
    let content: SwiftUI.Element = _skeletonValue()
    let inset: SwiftUI.Element = _skeletonValue()
    let edge: SwiftUI.Edge = _skeletonValue()
    let spacing: CoreGraphics.CGFloat = _skeletonValue()
    let alignment: SwiftUI.AlignmentKey = _skeletonValue()
    let isBar: Swift.Bool = _skeletonValue()
    final var scrollUnderBar: SwiftUI.ScrollElement? { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    init(content a0: SwiftUI.Element, inset a1: SwiftUI.Element, edge a2: SwiftUI.Edge, spacing a3: CoreGraphics.CGFloat, alignment a4: SwiftUI.AlignmentKey, isBar a5: Swift.Bool) { _skeleton() }
    deinit {}
}
