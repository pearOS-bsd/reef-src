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

public struct State<A> {
    public var _value: A
    public var _location: (SwiftUI.AnyLocation<A>)?
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(initialValue a0: A) { _skeleton() }
    public var projectedValue: SwiftUI.Binding<A> { _skeleton() }
    public init(wrappedValue a0: A) { _skeleton() }
}

extension SwiftUI.State where A: Swift.ExpressibleByNilLiteral {
    public init() { _skeleton() }
}

extension SwiftUI.State: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.State: SwiftUI.DynamicProperty {
}
