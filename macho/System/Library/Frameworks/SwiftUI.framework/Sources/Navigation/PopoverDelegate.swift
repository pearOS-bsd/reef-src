// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PopoverDelegate (Navigation).

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

class PopoverDelegate: NSObject {
    let isPresented: SwiftUI.Binding<Swift.Bool> = _skeletonValue()
    let closed: () -> () = _skeletonValue()
    init(_ a0: SwiftUI.Binding<Swift.Bool>, closed a1: @escaping () -> ()) { _skeleton() }
    deinit {}
}
