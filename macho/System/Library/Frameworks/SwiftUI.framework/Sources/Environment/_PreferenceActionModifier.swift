// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PreferenceActionModifier (Environment).

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

public struct _PreferenceActionModifier<A> where A: SwiftUI.PreferenceKey, A.Value: Swift.Equatable {
    public var action: (A.Value) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(action a0: @escaping (A.Value) -> ()) { _skeleton() }
}

extension SwiftUI._PreferenceActionModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._PreferenceActionModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
