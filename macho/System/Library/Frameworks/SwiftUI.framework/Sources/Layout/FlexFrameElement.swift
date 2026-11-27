// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FlexFrameElement (Layout).

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

class FlexFrameElement: SwiftUI.WrapperElement {
    let minWidth: CoreGraphics.CGFloat? = _skeletonValue()
    let idealWidth: CoreGraphics.CGFloat? = _skeletonValue()
    let maxWidth: CoreGraphics.CGFloat? = _skeletonValue()
    let minHeight: CoreGraphics.CGFloat? = _skeletonValue()
    let idealHeight: CoreGraphics.CGFloat? = _skeletonValue()
    let maxHeight: CoreGraphics.CGFloat? = _skeletonValue()
    let alignment: SwiftUI.Alignment = _skeletonValue()
    override var debugDetail: Swift.String { _skeleton() }
    override func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    init(_ a0: SwiftUI.Element, layout a1: SwiftUI._FlexFrameLayout) { _skeleton() }
    deinit {}
}
