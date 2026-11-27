// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityFocusState (State).

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

public struct AccessibilityFocusState<A> where A: Swift.Hashable {
    public var technologies: SwiftUI.AccessibilityTechnologies?
    public var value: A
    public var location: (SwiftUI.AnyLocation<A>)?
    public var resetValue: A
    public init() where A == Swift.Bool { _skeleton() }
    public init(`for` a0: SwiftUI.AccessibilityTechnologies) where A == Swift.Bool { _skeleton() }
    public init<A1>() where A == A1?, A1: Swift.Hashable { _skeleton() }
    public init<A1>(`for` a0: SwiftUI.AccessibilityTechnologies) where A == A1?, A1: Swift.Hashable { _skeleton() }
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.AccessibilityFocusState<A>.Binding { _skeleton() }
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public struct Binding {
        public var state: SwiftUI.Binding<A>
        public var wrappedValue: A {
            get { _skeleton() }
            set { _skeleton() }
        }
        public var projectedValue: SwiftUI.AccessibilityFocusState<A>.Binding { _skeleton() }
        init(state a0: SwiftUI.Binding<A>) { _skeleton() }
    }
}

extension SwiftUI.AccessibilityFocusState: SwiftUI.DynamicProperty {
}

extension SwiftUI.AccessibilityFocusState: SwiftUI.DynamicPropertyInstaller {
}
