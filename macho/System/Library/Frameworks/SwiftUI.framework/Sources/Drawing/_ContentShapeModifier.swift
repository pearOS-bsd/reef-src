// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ContentShapeModifier (Drawing).

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

public struct _ContentShapeModifier<A> where A: SwiftUI.Shape {
    public var shape: A
    public var eoFill: Swift.Bool
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(shape a0: A, eoFill a1: Swift.Bool) { _skeleton() }
}

extension SwiftUI._ContentShapeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ContentShapeModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
