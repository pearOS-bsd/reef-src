// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableRowElement (Navigation).

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

class TableRowElement: SwiftUI.Element {
    let cells: [SwiftUI.Element] = _skeletonValue()
    let columns: [SwiftUI.TableColumnEntry] = _skeletonValue()
    let isHeader: Swift.Bool = _skeletonValue()
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var children: [SwiftUI.Element] { _skeleton() }
    init(cells a0: [SwiftUI.Element], columns a1: [SwiftUI.TableColumnEntry], isHeader a2: Swift.Bool) { _skeleton() }
    deinit {}
}
