// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabViewElement (Navigation).

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

class TabViewElement: SwiftUI.Element {
    let bar: [SwiftUI.Element] = _skeletonValue()
    let page: SwiftUI.Element = _skeletonValue()
    let selected: Swift.Int = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    override var children: [SwiftUI.Element] { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    final var segmentWidth: CoreGraphics.CGFloat { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(bar a0: [SwiftUI.Element], page a1: SwiftUI.Element, selected a2: Swift.Int, environment a3: SwiftUI.EnvironmentValues) { _skeleton() }
    deinit {}
}
