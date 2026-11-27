// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SliderTickContentForEach (Controls).

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

public struct SliderTickContentForEach<A, B, C> where A: Swift.RandomAccessCollection, B: Swift.Hashable, C: SwiftUI.SliderTickContent {
    let data: A
    let content: (A.Element) -> C
    public init<A1>(_ a0: A, id a1: Swift.KeyPath<A1, B>, content a2: @escaping (A1) -> C) where A1 == A.Element, A.Element == C.Value { _skeleton() }
    public var _tickValues: [C.Value] { _skeleton() }
}

extension SwiftUI.SliderTickContentForEach: SwiftUI.SliderTickContent {
    public typealias Value = C.Value
}
