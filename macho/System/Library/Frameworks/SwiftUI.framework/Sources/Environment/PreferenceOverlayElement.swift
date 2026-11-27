// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreferenceOverlayElement (Environment).

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

class PreferenceOverlayElement: SwiftUI.Element {
    let content: SwiftUI.Element = _skeletonValue()
    let alignment: SwiftUI.Alignment = _skeletonValue()
    let build: ([SwiftUI.Element]) -> [SwiftUI.Element] = _skeletonValue()
    var overlay: [SwiftUI.Element] = _skeletonValue()
    init(content a0: SwiftUI.Element, alignment a1: SwiftUI.Alignment, build a2: @escaping ([SwiftUI.Element]) -> [SwiftUI.Element]) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    deinit {}
}
