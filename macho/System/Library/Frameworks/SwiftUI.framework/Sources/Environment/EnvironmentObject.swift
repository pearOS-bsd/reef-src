// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentObject (Environment).

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

@propertyWrapper public struct EnvironmentObject<A> where A: Combine.ObservableObject {
    public var _store: A?
    public var _seed: Swift.Int
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init() { _skeleton() }
    public var wrappedValue: A { _skeleton() }
    public var projectedValue: SwiftUI.EnvironmentObject<A>.Wrapper { _skeleton() }
    public func error() -> Swift.Never { _skeleton() }
    public struct Wrapper {
        public let root: A
        init(root a0: A) { _skeleton() }
        public subscript<A1>(dynamicMember a0: Swift.ReferenceWritableKeyPath<A, A1>) -> SwiftUI.Binding<A1> { _skeleton() }
    }
}

extension SwiftUI.EnvironmentObject: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.EnvironmentObject: SwiftUI.DynamicProperty {
}
