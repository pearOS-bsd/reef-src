// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// StateObject (State).

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

public struct StateObject<A> where A: Combine.ObservableObject {
    public var storage: SwiftUI.StateObject<A>.Storage
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(wrappedValue a0: @escaping @autoclosure () -> A) { _skeleton() }
    public var wrappedValue: A { _skeleton() }
    public var projectedValue: SwiftUI.ObservedObject<A>.Wrapper { _skeleton() }
    @frozen public enum Storage {
        case initially(() -> A)
        case object(SwiftUI.ObservedObject<A>)
    }
}

extension SwiftUI.StateObject: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.StateObject: SwiftUI.DynamicProperty {
}
