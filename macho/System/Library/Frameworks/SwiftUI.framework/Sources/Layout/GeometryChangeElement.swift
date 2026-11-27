// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryChangeElement (Layout).

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

class GeometryChangeElement: SwiftUI.WrapperElement {
    let check: (CGRect, SwiftUI.Element) -> () = _skeletonValue()
    override func place(`in` a0: CGRect) { _skeleton() }
    init(_ a0: SwiftUI.Element, check a1: @escaping (CGRect, SwiftUI.Element) -> ()) { _skeleton() }
    deinit {}
}
