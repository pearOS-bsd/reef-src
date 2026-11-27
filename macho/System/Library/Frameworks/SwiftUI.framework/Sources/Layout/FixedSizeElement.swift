// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FixedSizeElement (Layout).

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

class FixedSizeElement: SwiftUI.WrapperElement {
    let horizontal: Swift.Bool = _skeletonValue()
    let vertical: Swift.Bool = _skeletonValue()
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    init(_ a0: SwiftUI.Element, horizontal a1: Swift.Bool, vertical a2: Swift.Bool) { _skeleton() }
    deinit {}
}
