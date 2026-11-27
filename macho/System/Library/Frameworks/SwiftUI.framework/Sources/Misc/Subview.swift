// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Subview (Misc).

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

public struct Subview {
    public let id: SwiftUI.Subview.ID
    let element: SwiftUI.Element
    public struct ID {
        let value: Swift.AnyHashable
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.Subview.ID, _ a1: SwiftUI.Subview.ID) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.Subview.ID: Swift.Hashable {
}

extension SwiftUI.Subview.ID: Swift.Equatable {
}

extension SwiftUI.Subview: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.Subview: Swift.Identifiable {
}

extension SwiftUI.Subview: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}
