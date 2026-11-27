// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SpacerElement: the element of Spacer (flexible space along the axis of the stack that contains it).

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
    let minLength: CoreGraphics.CGFloat?
    var axis: SwiftUI.Axis? = nil
    init(minLength a0: CoreGraphics.CGFloat?) {
        minLength = a0
        super.init()
    }
    override var isSpacer: Swift.Bool { true }
    override var spacingKind: SwiftUI.SpacingKind { .spacer }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        // at least minLength (8 by default) along the stack's axis, nothing across it; both axes outside a stack
        let minimum = minLength ?? 8
        let width = max(minimum, a0.width ?? minimum)
        let height = max(minimum, a0.height ?? minimum)
        switch axis {
        case .horizontal?: return CGSize(width: width, height: 0)
        case .vertical?: return CGSize(width: 0, height: height)
        case nil: return CGSize(width: width, height: height)
        }
    }
    deinit {}
}
