// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AppStorage (State).

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

public struct AppStorage<A> {
    public var location: SwiftUI.AnyLocation<A>
    static func _reefInstall(at a0: Swift.UnsafeMutableRawPointer, context a1: SwiftUI.PropertyInstallContext) { _skeleton() }
    public init(wrappedValue a0: A, _ a1: Swift.String, store a2: UserDefaults?) where A: Swift.RawRepresentable, A.RawValue == Swift.String { _skeleton() }
    public init(wrappedValue a0: Swift.Bool, _ a1: Swift.String, store a2: UserDefaults?) where A == Swift.Bool { _skeleton() }
    public init(wrappedValue a0: Swift.Int, _ a1: Swift.String, store a2: UserDefaults?) where A == Swift.Int { _skeleton() }
    public init(wrappedValue a0: Swift.Double, _ a1: Swift.String, store a2: UserDefaults?) where A == Swift.Double { _skeleton() }
    public init(wrappedValue a0: Swift.String, _ a1: Swift.String, store a2: UserDefaults?) where A == Swift.String { _skeleton() }
    public init(wrappedValue a0: Foundation.URL, _ a1: Swift.String, store a2: UserDefaults?) where A == Foundation.URL { _skeleton() }
    public init(wrappedValue a0: Foundation.Data, _ a1: Swift.String, store a2: UserDefaults?) where A == Foundation.Data { _skeleton() }
    public init(wrappedValue a0: A, _ a1: Swift.String, store a2: UserDefaults?) where A: Swift.RawRepresentable, A.RawValue == Swift.Int { _skeleton() }
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.Binding<A> { _skeleton() }
}

extension SwiftUI.AppStorage where A: Swift.ExpressibleByNilLiteral {
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Swift.Bool? { _skeleton() }
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Swift.Int? { _skeleton() }
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Swift.Double? { _skeleton() }
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Swift.String? { _skeleton() }
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Foundation.URL? { _skeleton() }
    public init(_ a0: Swift.String, store a1: UserDefaults?) where A == Foundation.Data? { _skeleton() }
}

extension SwiftUI.AppStorage: SwiftUI.DynamicPropertyInstaller {
}

extension SwiftUI.AppStorage: SwiftUI.DynamicProperty {
}
