// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListScrollElement (Lists).

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

class ListScrollElement: SwiftUI.PlatformViewElement {
    let holder: SwiftUI.ScrollHolder = _skeletonValue()
    let content: SwiftUI.Element = _skeletonValue()
    var background: SwiftUI.Color.Resolved? = _skeletonValue()
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    init(holder a0: SwiftUI.ScrollHolder, content a1: SwiftUI.Element, canvas a2: SwiftUI.ElementCanvas?) { _skeleton() }
    deinit {}
}
