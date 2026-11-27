// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// State (State).

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

@propertyWrapper public struct State<A> {
    public var _value: A
    public var _location: (SwiftUI.AnyLocation<A>)?
    /// the stored location once the view is installed in a body node; before that the initial value (and sets are lost)
    public var wrappedValue: A {
        get { _location?.get() ?? _value }
        nonmutating set { _location?.set(newValue, transaction: SwiftUI.Transaction()) }
    }
    /// give the State in the view at a0 its persistent location in the body node
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) {
        let state = a0.assumingMemoryBound(to: SwiftUI.State<A>.self)
        let initial = state.pointee._value
        let location = a1.node.slot(a1.index) { SwiftUI.StoredLocation(initial) }
        state.pointee._location = location
    }
    public init(initialValue a0: A) {
        _value = a0
        _location = nil
    }
    public var projectedValue: SwiftUI.Binding<A> {
        SwiftUI.Binding(location: _location ?? SwiftUI.ConstantLocation(_value), transaction: SwiftUI.Transaction())
    }
    public init(wrappedValue a0: A) {
        _value = a0
        _location = nil
    }
}

extension SwiftUI.State where A: Swift.ExpressibleByNilLiteral {
    public init() { self.init(wrappedValue: nil) }
}

extension SwiftUI.State: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.State: SwiftUI.DynamicProperty {
}
