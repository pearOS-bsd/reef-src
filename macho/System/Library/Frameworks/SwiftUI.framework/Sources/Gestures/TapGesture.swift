// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TapGesture (Gestures).

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

public struct TapGesture {
    public var count: Swift.Int
    public init(count a0: Swift.Int) { _skeleton() }
}

extension SwiftUI.TapGesture: SwiftUI.RecognizableGesture {
    func _makeRecognizer() -> SwiftUI.GestureRecognizer? { _skeleton() }
}

extension SwiftUI.TapGesture: SwiftUI.Gesture {
    public typealias Value = ()
    public typealias Body = Swift.Never
}
