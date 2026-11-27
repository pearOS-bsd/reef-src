// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SplitElement (Navigation).

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

class SplitElement: SwiftUI.Element {
    let axis: SwiftUI.Axis = _skeletonValue()
    let panes: [SwiftUI.Element] = _skeletonValue()
    let fixed: [CoreGraphics.CGFloat?] = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    init(axis a0: SwiftUI.Axis, panes a1: [SwiftUI.Element], fixed a2: [CoreGraphics.CGFloat?]?, environment a3: SwiftUI.EnvironmentValues) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    final func lengths(_ a0: CoreGraphics.CGFloat) -> [CoreGraphics.CGFloat] { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    deinit {}
}
