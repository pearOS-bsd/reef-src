// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _MapGesture (Gestures).

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

public struct _MapGesture<A, B> where A: SwiftUI.Gesture {
    let content: A
    let transform: (A.Value) -> B
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
    init(content a0: A, transform a1: @escaping (A.Value) -> B) { _skeleton() }
}

extension SwiftUI._MapGesture: SwiftUI.RecognizableGesture {
}

extension SwiftUI._MapGesture: SwiftUI.Gesture {
    public typealias Value = B
    public typealias Body = Swift.Never
}
