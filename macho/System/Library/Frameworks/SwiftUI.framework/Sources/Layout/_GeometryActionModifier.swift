// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _GeometryActionModifier (Layout).

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

public struct _GeometryActionModifier<A> where A: Swift.Equatable {
    var value: (SwiftUI.GeometryProxy) -> A
    var action: (A) -> ()
    public init(value a0: @escaping (SwiftUI.GeometryProxy) -> A, action a1: @escaping (A) -> ()) { _skeleton() }
}

extension SwiftUI._GeometryActionModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
