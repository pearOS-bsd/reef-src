// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LazyState (State).

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

@propertyWrapper public struct LazyState<A> {
    public var storage: SwiftUI.LazyState<A>.Storage
    public var location: AnyObject?
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(wrappedValue a0: @escaping @autoclosure () -> A) { _skeleton() }
    var stored: (SwiftUI.StoredLocation<A>)? { _skeleton() }
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.Binding<A> { _skeleton() }
    public enum Storage {
        case value(A)
        case initializer(() -> A)
        var initialValue: A { _skeleton() }
    }
}

extension SwiftUI.LazyState: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.LazyState: SwiftUI.DynamicProperty {
}
