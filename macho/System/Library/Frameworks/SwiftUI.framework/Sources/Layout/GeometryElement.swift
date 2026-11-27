// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryElement (Layout).

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

class GeometryElement: SwiftUI.Element {
    let build: (CGRect, SwiftUI.GeometryElement) -> [SwiftUI.Element] = _skeletonValue()
    var built: [SwiftUI.Element] = _skeletonValue()
    override func place(`in` a0: CGRect) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    init(build a0: @escaping (CGRect, SwiftUI.GeometryElement) -> [SwiftUI.Element]) { _skeleton() }
    deinit {}
}
