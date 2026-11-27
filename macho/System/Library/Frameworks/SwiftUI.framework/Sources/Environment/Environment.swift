// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Environment (Environment).

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

public struct Environment<A> {
    public var content: SwiftUI.Environment<A>.Content
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(_ a0: Swift.KeyPath<SwiftUI.EnvironmentValues, A>) { _skeleton() }
    public var wrappedValue: A { _skeleton() }
    public init(_ a0: A.Type) where A: AnyObject, A: Observation.Observable { _skeleton() }
    public init<A1>(_ a0: A1.Type) where A == A1?, A1: AnyObject, A1: Observation.Observable { _skeleton() }
    @frozen public enum Content {
        case keyPath(Swift.KeyPath<SwiftUI.EnvironmentValues, A>)
        case value(A)
    }
}

extension SwiftUI.Environment: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.Environment: SwiftUI.DynamicProperty {
}
