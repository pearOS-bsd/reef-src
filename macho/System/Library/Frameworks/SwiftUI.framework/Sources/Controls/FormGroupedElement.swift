// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FormGroupedElement (Controls).

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

class FormGroupedElement: SwiftUI.Element {
    let groups: [SwiftUI.FormGroupedElement.Group] = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    var fitsContentHeight: Swift.Bool = _skeletonValue()
    var boxes: [CGRect] = _skeletonValue()
    var separators: [CGRect] = _skeletonValue()
    init(elements a0: [SwiftUI.Element], environment a1: SwiftUI.EnvironmentValues) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    final var small: Swift.Bool { _skeleton() }
    final func layout(_ a0: CoreGraphics.CGFloat?, place a1: CGPoint?) -> CGSize { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    deinit {}
    struct Group {
        let header: [SwiftUI.Element]
        let rows: [SwiftUI.Element]
        let footer: [SwiftUI.Element]
    }
}
