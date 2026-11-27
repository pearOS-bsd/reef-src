// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ChangedGesture (Gestures).

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

public struct _ChangedGesture<A> where A: SwiftUI.Gesture, A.Value: Swift.Equatable {
    public var _body: A
    public var action: (A.Value) -> ()
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    public init(_body a0: A, action a1: @escaping (A.Value) -> ()) { _skeleton() }
}

extension SwiftUI._ChangedGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI._ChangedGesture: SwiftUI.Gesture {
    public typealias Value = A.Value
    public typealias Body = Swift.Never
}
