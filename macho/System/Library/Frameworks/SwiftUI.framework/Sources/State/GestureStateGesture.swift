// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GestureStateGesture (State).

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

public struct GestureStateGesture<A, B> where A: SwiftUI.Gesture {
    public var base: A
    public var state: SwiftUI.GestureState<B>
    public var body: (A.Value, inout B, inout SwiftUI.Transaction) -> ()
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    public init(base a0: A, state a1: SwiftUI.GestureState<B>, body a2: @escaping (A.Value, inout B, inout SwiftUI.Transaction) -> ()) { _skeleton() }
}

extension SwiftUI.GestureStateGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI.GestureStateGesture: SwiftUI.Gesture {
    public typealias Value = A.Value
    public typealias Body = Swift.Never
}
