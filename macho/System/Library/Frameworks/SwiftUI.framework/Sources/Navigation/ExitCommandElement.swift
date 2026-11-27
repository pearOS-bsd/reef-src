// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ExitCommandElement (Navigation).

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

class ExitCommandElement: SwiftUI.WrapperElement {
    let action: () -> () = _skeletonValue()
    init(_ a0: SwiftUI.Element, action a1: @escaping () -> ()) { _skeleton() }
    deinit {}
}
