// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Transaction (Animation).

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

public struct Transaction {
    public var plist: SwiftUI.PropertyList
    public init() { plist = SwiftUI.PropertyList() }
    public init(animation a0: SwiftUI.Animation?) {
        plist = SwiftUI.PropertyList()
        animation = a0
    }
    public func addAnimationCompletion(criteria a0: SwiftUI.AnimationCompletionCriteria, _ a1: @escaping () -> ()) { _skeleton() }
    public var animation: SwiftUI.Animation? {
        get { self[SwiftUI.AnimationTransactionKey.self] }
        set { self[SwiftUI.AnimationTransactionKey.self] = newValue }
    }
    /// the values live in one TransactionValues element of the property list, keyed by key type
    public subscript<A>(_ a0: A.Type) -> A.Value where A: SwiftUI.TransactionKey {
        get {
            if let values = plist.elements as? SwiftUI.TransactionValues, let value = values.values[ObjectIdentifier(a0)] as? A.Value {
                return value
            }
            return A.defaultValue
        }
        set { plist.elements = SwiftUI.TransactionStack.setting(newValue, for: ObjectIdentifier(a0), in: plist) }
    }
    public var disablesAnimations: Swift.Bool {
        get { self[SwiftUI.DisablesAnimationsKey.self] }
        set { self[SwiftUI.DisablesAnimationsKey.self] = newValue }
    }
    public var isContinuous: Swift.Bool {
        get { self[SwiftUI.ContinuousTransactionKey.self] }
        set { self[SwiftUI.ContinuousTransactionKey.self] = newValue }
    }
}
