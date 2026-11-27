// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicPropertyLayout (Graph).

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

enum DynamicPropertyLayout {
    static func install<A>(_ a0: inout A, node a1: SwiftUI.BodyNode, environment a2: SwiftUI.EnvironmentValues) { _skeleton() }
    static func entries(of a0: Any.Type) -> [SwiftUI.DynamicPropertyLayout.Entry] { _skeleton() }
    static func collect(_ a0: Any.Type, base a1: Swift.Int, into a2: inout [SwiftUI.DynamicPropertyLayout.Entry]) { _skeleton() }
    static var cache: [Swift.ObjectIdentifier: [SwiftUI.DynamicPropertyLayout.Entry]] { _skeleton() }
    enum Entry {
        case install(offset: Swift.Int, type: any SwiftUI.DynamicPropertyInstaller.Type)
        case update(offset: Swift.Int, type: any SwiftUI.DynamicProperty.Type)
    }
}
