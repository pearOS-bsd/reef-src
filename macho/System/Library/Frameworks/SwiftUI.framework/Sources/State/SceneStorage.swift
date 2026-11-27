// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SceneStorage (State).

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

@propertyWrapper public struct SceneStorage<A> {
    let state: SwiftUI.State<A>
    public init(wrappedValue a0: Swift.Bool, _ a1: Swift.String) where A == Swift.Bool { _skeleton() }
    public init(wrappedValue a0: Swift.Int, _ a1: Swift.String) where A == Swift.Int { _skeleton() }
    public init(wrappedValue a0: Swift.Double, _ a1: Swift.String) where A == Swift.Double { _skeleton() }
    public init(wrappedValue a0: Swift.String, _ a1: Swift.String) where A == Swift.String { _skeleton() }
    public init(wrappedValue a0: Foundation.URL, _ a1: Swift.String) where A == Foundation.URL { _skeleton() }
    public init(wrappedValue a0: Foundation.Data, _ a1: Swift.String) where A == Foundation.Data { _skeleton() }
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.Binding<A> { _skeleton() }
}

extension SwiftUI.SceneStorage: SwiftUI.DynamicProperty {
}
