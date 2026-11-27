// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ChangeModifier (Views).

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

struct ChangeModifier<A> where A: Swift.Equatable {
    let value: A
    let initial: Swift.Bool
    let action: () -> ()
    init(value a0: A, initial a1: Swift.Bool, action a2: @escaping () -> ()) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ChangeModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.ChangeModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
