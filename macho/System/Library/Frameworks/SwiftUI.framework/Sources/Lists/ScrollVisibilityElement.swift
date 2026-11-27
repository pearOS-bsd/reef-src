// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollVisibilityElement (Lists).

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

class ScrollVisibilityElement: SwiftUI.WrapperElement {
    let threshold: Swift.Double = _skeletonValue()
    let state: SwiftUI.ScrollVisibilityState = _skeletonValue()
    let action: (Swift.Bool) -> () = _skeletonValue()
    override func place(`in` a0: CGRect) { _skeleton() }
    final func check() { _skeleton() }
    init(_ a0: SwiftUI.Element, threshold a1: Swift.Double, state a2: SwiftUI.ScrollVisibilityState, action a3: @escaping (Swift.Bool) -> ()) { _skeleton() }
    deinit {}
}
