// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationState (State).

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

public struct AnimationState<A> where A: SwiftUI.VectorArithmetic {
    var values: [Swift.ObjectIdentifier: Any]
    public init() { _skeleton() }
    public subscript<A1>(_ a0: A1.Type) -> A1.Value where A1: SwiftUI.AnimationStateKey {
        get { _skeleton() }
        set { _skeleton() }
    }
}
