// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AspectRatioElement (Layout).

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

class AspectRatioElement: SwiftUI.WrapperElement {
    let ratio: CoreGraphics.CGFloat? = _skeletonValue()
    let mode: SwiftUI.ContentMode = _skeletonValue()
    final func fitted(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    init(_ a0: SwiftUI.Element, ratio a1: CoreGraphics.CGFloat?, mode a2: SwiftUI.ContentMode) { _skeleton() }
    deinit {}
}
