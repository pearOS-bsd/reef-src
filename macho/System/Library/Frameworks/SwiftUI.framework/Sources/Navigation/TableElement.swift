// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableElement (Navigation).

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

class TableElement: SwiftUI.Element {
    let header: SwiftUI.TableRowElement = _skeletonValue()
    let body: SwiftUI.ListScrollElement = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    let filled: Swift.Bool = _skeletonValue()
    let headerHidden: Swift.Bool = _skeletonValue()
    override var children: [SwiftUI.Element] { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    init(header a0: SwiftUI.TableRowElement, body a1: SwiftUI.ListScrollElement, environment a2: SwiftUI.EnvironmentValues, filled a3: Swift.Bool) { _skeleton() }
    deinit {}
}
