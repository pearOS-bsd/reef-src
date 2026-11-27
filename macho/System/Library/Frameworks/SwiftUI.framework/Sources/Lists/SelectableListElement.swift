// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SelectableListElement (Lists).

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

class SelectableListElement: SwiftUI.WrapperElement {
    let interaction: SwiftUI.ListSelectInteraction = _skeletonValue()
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    init(_ a0: SwiftUI.ListContentElement) { _skeleton() }
    deinit {}
}

extension SwiftUI.SelectableListElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
