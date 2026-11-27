// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlViewElement (Controls).

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

final class ControlViewElement: SwiftUI.PlatformViewElement {
    var resizeWhenPlaced: (() -> ())? = _skeletonValue()
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func viewFrame(`for` a0: CGRect) -> CGRect { _skeleton() }
    deinit {}
}
