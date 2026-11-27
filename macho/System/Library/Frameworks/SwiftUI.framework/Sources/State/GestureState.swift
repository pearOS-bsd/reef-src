// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureState (State).

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

@propertyWrapper public struct GestureState<A> {
    public var _state: SwiftUI.State<A>
    public var _reset: (A, inout SwiftUI.Transaction) -> ()
    public init(wrappedValue a0: A) { _skeleton() }
    public init(initialValue a0: A) { _skeleton() }
    public init(wrappedValue a0: A, resetTransaction a1: SwiftUI.Transaction) { _skeleton() }
    public var wrappedValue: A { _skeleton() }
    public var projectedValue: SwiftUI.GestureState<A> { _skeleton() }
}

extension SwiftUI.GestureState: SwiftUI.DynamicProperty {
}
