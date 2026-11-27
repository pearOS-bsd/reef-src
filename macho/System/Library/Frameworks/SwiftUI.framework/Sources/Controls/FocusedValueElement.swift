// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedValueElement (Controls).

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

class FocusedValueElement: SwiftUI.WrapperElement {
    let isScene: Swift.Bool = _skeletonValue()
    let write: (inout SwiftUI.FocusedValues) -> () = _skeletonValue()
    init(_ a0: SwiftUI.Element, isScene a1: Swift.Bool, write a2: @escaping (inout SwiftUI.FocusedValues) -> ()) { _skeleton() }
    deinit {}
}
