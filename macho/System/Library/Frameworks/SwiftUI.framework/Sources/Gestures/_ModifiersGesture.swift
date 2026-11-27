// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ModifiersGesture (Gestures).

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

public struct _ModifiersGesture<A> where A: SwiftUI.Gesture {
    var content: A
    var modifiers: SwiftUI.EventModifiers
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    init(content a0: A, modifiers a1: SwiftUI.EventModifiers) { _skeleton() }
}

extension SwiftUI._ModifiersGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI._ModifiersGesture: SwiftUI.Gesture {
    public typealias Value = A.Value
    public typealias Body = Swift.Never
}
