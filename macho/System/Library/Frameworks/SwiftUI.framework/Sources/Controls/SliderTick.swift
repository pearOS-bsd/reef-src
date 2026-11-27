// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SliderTick (Controls).

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

public struct SliderTick<A> where A: Swift.BinaryFloatingPoint {
    let value: A
    let label: SwiftUI.AnyView?
    public init(_ a0: A) { _skeleton() }
    public init<A1>(_ a0: A, label a1: () -> A1) where A1: SwiftUI.View { _skeleton() }
    public var _tickValues: [A] { _skeleton() }
}

extension SwiftUI.SliderTick: SwiftUI.SliderTickContent {
    public typealias Value = A
}
