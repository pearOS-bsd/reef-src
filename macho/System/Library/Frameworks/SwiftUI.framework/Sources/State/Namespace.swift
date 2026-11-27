// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Namespace (State).

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

@propertyWrapper public struct Namespace {
    public var id: Swift.Int
    public init() { _skeleton() }
    public var wrappedValue: SwiftUI.Namespace.ID { _skeleton() }
    public struct ID {
        var id: Swift.Int
        public static func == (_ a0: SwiftUI.Namespace.ID, _ a1: SwiftUI.Namespace.ID) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
    }
}

extension SwiftUI.Namespace: SwiftUI.DynamicPropertyInstaller {
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
}

extension SwiftUI.Namespace: SwiftUI.DynamicProperty {
}

extension SwiftUI.Namespace.ID: Swift.Hashable {
}

extension SwiftUI.Namespace.ID: Swift.Equatable {
}
