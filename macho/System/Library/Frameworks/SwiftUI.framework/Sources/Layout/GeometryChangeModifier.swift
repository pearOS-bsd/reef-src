// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryChangeModifier (Layout).

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

struct GeometryChangeModifier<A> where A: Swift.Equatable {
    let transform: (SwiftUI.GeometryProxy) -> A
    let action: (A, A) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    init(transform a0: @escaping (SwiftUI.GeometryProxy) -> A, action a1: @escaping (A, A) -> ()) { _skeleton() }
}

extension SwiftUI.GeometryChangeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.GeometryChangeModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
