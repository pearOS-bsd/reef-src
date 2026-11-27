// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PositionElement (Layout).

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

class PositionElement: SwiftUI.WrapperElement {
    let position: CGPoint = _skeletonValue()
    override func place(`in` a0: CGRect) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    init(_ a0: SwiftUI.Element, position a1: CGPoint) { _skeleton() }
    deinit {}
}
