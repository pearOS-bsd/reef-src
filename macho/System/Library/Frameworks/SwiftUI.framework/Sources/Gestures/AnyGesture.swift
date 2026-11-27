// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyGesture (Gestures).

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

public struct AnyGesture<A> {
    let makeRecognizer: () -> SwiftUI.GestureRecognizer?
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    public init<A1>(_ a0: A1) where A == A1.Value, A1: SwiftUI.Gesture { _skeleton() }
}

extension SwiftUI.AnyGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI.AnyGesture: SwiftUI.Gesture {
    public typealias Value = A
    public typealias Body = Swift.Never
}
