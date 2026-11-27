// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollElement (Lists).

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

class ScrollElement: SwiftUI.PlatformViewElement {
    let holder: SwiftUI.ScrollHolder = _skeletonValue()
    let axes: SwiftUI.Axis.Set = _skeletonValue()
    let content: SwiftUI.Element = _skeletonValue()
    let anchors: SwiftUI.ScrollAnchors = _skeletonValue()
    final var barInsets: SwiftUI.EdgeInsets {
        get { _skeleton() }
        set { _skeleton() }
    }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(holder a0: SwiftUI.ScrollHolder, axes a1: SwiftUI.Axis.Set, content a2: SwiftUI.Element, canvas a3: SwiftUI.ElementCanvas?, anchors a4: SwiftUI.ScrollAnchors) { _skeleton() }
    deinit {}
}
