// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Gesture (Gestures).

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

public protocol Gesture {
    associatedtype Value
    associatedtype Body: SwiftUI.Gesture
    var body: Self.Body { get }
}

extension SwiftUI.Gesture where Self.Body == Swift.Never {
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.Gesture {
    public func onEnded(_ a0: @escaping (Self.Value) -> ()) -> SwiftUI._EndedGesture<Self> { _skeleton() }
    public func simultaneously<A1>(with a0: A1) -> SwiftUI.SimultaneousGesture<Self, A1> where A1: SwiftUI.Gesture { _skeleton() }
    public func exclusively<A1>(before a0: A1) -> SwiftUI.ExclusiveGesture<Self, A1> where A1: SwiftUI.Gesture { _skeleton() }
    public func sequenced<A1>(before a0: A1) -> SwiftUI.SequenceGesture<Self, A1> where A1: SwiftUI.Gesture { _skeleton() }
    public func map<A1>(_ a0: @escaping (Self.Value) -> A1) -> SwiftUI._MapGesture<Self, A1> { _skeleton() }
    public func updating<A1>(_ a0: SwiftUI.GestureState<A1>, body a1: @escaping (Self.Value, inout A1, inout SwiftUI.Transaction) -> ()) -> SwiftUI.GestureStateGesture<Self, A1> { _skeleton() }
    public func modifiers(_ a0: SwiftUI.EventModifiers) -> SwiftUI._ModifiersGesture<Self> { _skeleton() }
}

extension SwiftUI.Gesture where Self.Value: Swift.Equatable {
    public func onChanged(_ a0: @escaping (Self.Value) -> ()) -> SwiftUI._ChangedGesture<Self> { _skeleton() }
}
