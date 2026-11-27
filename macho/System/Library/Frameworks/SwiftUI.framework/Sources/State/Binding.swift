// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Binding (State).

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

@propertyWrapper public struct Binding<A> {
    public var transaction: SwiftUI.Transaction
    public var location: SwiftUI.AnyLocation<A>
    public var _value: A
    public var wrappedValue: A {
        get { location.get() }
        nonmutating set { location.set(newValue, transaction: transaction) }
    }
    init(location a0: SwiftUI.AnyLocation<A>, transaction a1: SwiftUI.Transaction) {
        transaction = a1
        location = a0
        _value = a0.get()
    }
    public init(get a0: @escaping () -> A, set a1: @escaping (A) -> ()) { _skeleton() }
    static func sameBinding(_ a0: Swift.UnsafeRawPointer, _ a1: Swift.UnsafeRawPointer) -> Swift.Bool { _skeleton() }
    public init?(_ a0: SwiftUI.Binding<A?>) { _skeleton() }
    public init<A1>(_ a0: SwiftUI.Binding<A1>) where A == A1? { _skeleton() }
    public init<A1>(_ a0: SwiftUI.Binding<A1>) where A == Swift.AnyHashable, A1: Swift.Hashable { _skeleton() }
    public static func constant(_ a0: A) -> SwiftUI.Binding<A> { _skeleton() }
    public init(get a0: @escaping () -> A, set a1: @escaping (A, SwiftUI.Transaction) -> ()) { _skeleton() }
    public subscript<A1>(dynamicMember a0: Swift.WritableKeyPath<A, A1>) -> SwiftUI.Binding<A1> { _skeleton() }
    public func transaction(_ a0: SwiftUI.Transaction) -> SwiftUI.Binding<A> { _skeleton() }
    public func animation(_ a0: SwiftUI.Animation?) -> SwiftUI.Binding<A> { _skeleton() }
    public var projectedValue: SwiftUI.Binding<A> { _skeleton() }
}

extension SwiftUI.Binding where A: Swift.Identifiable {
    public var id: A.ID { _skeleton() }
}

extension SwiftUI.Binding where A: Swift.MutableCollection {
    public var startIndex: A.Index { _skeleton() }
    public var endIndex: A.Index { _skeleton() }
    public var indices: A.Indices { _skeleton() }
    public func index(after a0: A.Index) -> A.Index { _skeleton() }
    public func formIndex(after a0: inout A.Index) { _skeleton() }
    public subscript(_ a0: A.Index) -> SwiftUI.Binding<A.Element> { _skeleton() }
}

extension SwiftUI.Binding where A: Swift.BidirectionalCollection, A: Swift.MutableCollection {
    public func index(before a0: A.Index) -> A.Index { _skeleton() }
    public func formIndex(before a0: inout A.Index) { _skeleton() }
}

extension SwiftUI.Binding: SwiftUI.BindingField {
}

extension SwiftUI.Binding: Swift.Identifiable where A: Swift.Identifiable {
    public typealias ID = A.ID
}

extension SwiftUI.Binding: Swift.Sequence where A: Swift.MutableCollection {
    public typealias Element = SwiftUI.Binding<A.Element>
    public typealias Iterator = Swift.IndexingIterator<SwiftUI.Binding<A>>
}

extension SwiftUI.Binding: Swift.Collection where A: Swift.MutableCollection {
    public typealias Index = A.Index
    public typealias SubSequence = Swift.Slice<SwiftUI.Binding<A>>
    public typealias Indices = A.Indices
}

extension SwiftUI.Binding: Swift.BidirectionalCollection where A: Swift.BidirectionalCollection, A: Swift.MutableCollection {
}

extension SwiftUI.Binding: Swift.RandomAccessCollection where A: Swift.MutableCollection, A: Swift.RandomAccessCollection {
}
