// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ValueActionModifier (Views).

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

public struct _ValueActionModifier<A> where A: Swift.Equatable {
    public var value: A
    public var action: (A) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(value a0: A, action a1: @escaping (A) -> ()) { _skeleton() }
}

extension SwiftUI._ValueActionModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._ValueActionModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
