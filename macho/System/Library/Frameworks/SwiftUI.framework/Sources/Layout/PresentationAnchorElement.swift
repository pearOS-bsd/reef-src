// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationAnchorElement (Layout).

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

class PresentationAnchorElement: SwiftUI.WrapperElement {
    let holder: SwiftUI.PresentationHolder = _skeletonValue()
    var contextMenu: (() -> [SwiftUI.MenuEntry])? = _skeletonValue()
    var help: Swift.String? = _skeletonValue()
    override func place(`in` a0: CGRect) { _skeleton() }
    init(_ a0: SwiftUI.Element, holder a1: SwiftUI.PresentationHolder) { _skeleton() }
    deinit {}
}
