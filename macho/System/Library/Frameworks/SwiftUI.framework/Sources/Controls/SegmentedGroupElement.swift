// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SegmentedGroupElement (Controls).

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

class SegmentedGroupElement: SwiftUI.Element {
    let segments: [SwiftUI.Element] = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    final func widths(`in` a0: CoreGraphics.CGFloat?) -> [CoreGraphics.CGFloat] { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    init(segments a0: [SwiftUI.Element], environment a1: SwiftUI.EnvironmentValues) { _skeleton() }
    deinit {}
}
