// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusState (State).

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

public struct FocusState<A> where A: Swift.Hashable {
    public var value: A
    public var location: (SwiftUI.AnyLocation<A>)?
    public var resetValue: A
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.FocusState<A>.Binding { _skeleton() }
    public init<A1>() where A == A1?, A1: Swift.Hashable { _skeleton() }
    public init() where A == Swift.Bool { _skeleton() }
    public struct Binding {
        public var binding: SwiftUI.Binding<A>
        public init(binding a0: SwiftUI.Binding<A>) { _skeleton() }
        public var wrappedValue: A {
            get { _skeleton() }
            set { _skeleton() }
        }
        public var projectedValue: SwiftUI.FocusState<A>.Binding { _skeleton() }
    }
}

extension SwiftUI.FocusState: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.FocusState: SwiftUI.DynamicProperty {
}
