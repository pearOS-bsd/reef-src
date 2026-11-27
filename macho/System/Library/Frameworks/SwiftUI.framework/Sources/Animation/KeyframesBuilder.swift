// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframesBuilder (Animation).

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

public struct KeyframesBuilder<A> {
    public static func buildPartialBlock<A1, B1>(accumulated a0: A1, next a1: B1) -> some Any where A == A1.Value, A1: SwiftUI.Keyframes, B1: SwiftUI.Keyframes, A1.Value == B1.Value { _skeleton() }
    public static func buildPartialBlock<A1>(first a0: A1) -> A1 where A == A1.Value, A1: SwiftUI.Keyframes { _skeleton() }
    public static func buildFinalResult<A1>(_ a0: A1) -> A1 where A == A1.Value, A1: SwiftUI.Keyframes { _skeleton() }
    public static func buildExpression<A1>(_ a0: A1) -> A1 where A == A1.Value, A1: SwiftUI.Keyframes { _skeleton() }
}
