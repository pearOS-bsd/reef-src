// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpacerElement (Layout).

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

class SpacerElement: SwiftUI.Element {
    let minLength: CoreGraphics.CGFloat? = _skeletonValue()
    var axis: SwiftUI.Axis? = _skeletonValue()
    init(minLength a0: CoreGraphics.CGFloat?) { _skeleton() }
    override var isSpacer: Swift.Bool { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    deinit {}
}
