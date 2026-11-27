// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Representables: NSViewRepresentable and NSViewControllerRepresentable values made into elements.

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

enum Representables {
    static func build<A>(_ a0: A, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A: SwiftUI.NSViewRepresentable { _skeleton() }
    static func build<A>(_ a0: A, _ a1: SwiftUI.BuildContext) -> [SwiftUI.Element] where A: SwiftUI.NSViewControllerRepresentable { _skeleton() }
    static func installProperties<A>(_ a0: A, _ a1: SwiftUI.BuildContext) -> A { _skeleton() }
    static func atLeastRequired(_ a0: CGSize, of a1: NSView) -> CGSize { _skeleton() }
}
