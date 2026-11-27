// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedBinding (State).

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

public struct FocusedBinding<A> {
    let keyPath: Swift.KeyPath<SwiftUI.FocusedValues, (SwiftUI.Binding<A>)?>
    public var wrappedValue: A? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var projectedValue: SwiftUI.Binding<A?> { _skeleton() }
    public init(_ a0: Swift.KeyPath<SwiftUI.FocusedValues, (SwiftUI.Binding<A>)?>) { _skeleton() }
}

extension SwiftUI.FocusedBinding: SwiftUI.DynamicProperty {
}
