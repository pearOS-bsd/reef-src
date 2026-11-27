// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FrameElement (Layout).

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

class FrameElement: SwiftUI.WrapperElement {
    let width: CoreGraphics.CGFloat? = _skeletonValue()
    let height: CoreGraphics.CGFloat? = _skeletonValue()
    let alignment: SwiftUI.Alignment = _skeletonValue()
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool { _skeleton() }
    init(_ a0: SwiftUI.Element, width a1: CoreGraphics.CGFloat?, height a2: CoreGraphics.CGFloat?, alignment a3: SwiftUI.Alignment) { _skeleton() }
    deinit {}
}
