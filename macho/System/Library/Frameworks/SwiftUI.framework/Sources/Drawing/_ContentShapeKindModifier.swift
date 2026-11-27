// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ContentShapeKindModifier (Drawing).

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

public struct _ContentShapeKindModifier<A> where A: SwiftUI.Shape {
    public var shape: A
    public var eoFill: Swift.Bool
    public var kinds: SwiftUI.ContentShapeKinds
    public init(shape a0: A, eoFill a1: Swift.Bool, kinds a2: SwiftUI.ContentShapeKinds) { _skeleton() }
}

extension SwiftUI._ContentShapeKindModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
