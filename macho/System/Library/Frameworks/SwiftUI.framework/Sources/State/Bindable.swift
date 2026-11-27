// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Bindable (State).

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

@propertyWrapper public struct Bindable<A> {
    var value: A
    public var wrappedValue: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    init(storing a0: A) { _skeleton() }
    public var projectedValue: SwiftUI.Bindable<A> { _skeleton() }
}

extension SwiftUI.Bindable where A: AnyObject {
    public subscript<A1>(dynamicMember a0: Swift.ReferenceWritableKeyPath<A, A1>) -> SwiftUI.Binding<A1> { _skeleton() }
}

extension SwiftUI.Bindable where A: AnyObject, A: Observation.Observable {
    public init(wrappedValue a0: A) { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}
