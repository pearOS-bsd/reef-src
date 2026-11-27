// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationContext (Navigation).

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

class NavigationContext {
    var destinations: [Swift.ObjectIdentifier: (Swift.AnyHashable) -> SwiftUI.AnyView?] = _skeletonValue()
    var push: (SwiftUI.NavigationEntry) -> () = _skeletonValue()
    var selectedValue: Swift.AnyHashable? = _skeletonValue()
    final func resolve(_ a0: SwiftUI.NavigationEntry) -> SwiftUI.AnyView? { _skeleton() }
    init() { _skeleton() }
    deinit {}
}
