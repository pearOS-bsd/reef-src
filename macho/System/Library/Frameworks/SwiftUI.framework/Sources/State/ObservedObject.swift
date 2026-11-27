// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ObservedObject (State).

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

@propertyWrapper public struct ObservedObject<A> where A: Combine.ObservableObject {
    public var _seed: Swift.Int
    public var wrappedValue: A
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(initialValue a0: A) { _skeleton() }
    public init(wrappedValue a0: A) { _skeleton() }
    public var projectedValue: SwiftUI.ObservedObject<A>.Wrapper { _skeleton() }
    public struct Wrapper {
        public let root: A
        public init(root a0: A) { _skeleton() }
        public subscript<A1>(dynamicMember a0: Swift.ReferenceWritableKeyPath<A, A1>) -> SwiftUI.Binding<A1> { _skeleton() }
    }
}

extension SwiftUI.ObservedObject: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.ObservedObject: SwiftUI.DynamicProperty {
}
