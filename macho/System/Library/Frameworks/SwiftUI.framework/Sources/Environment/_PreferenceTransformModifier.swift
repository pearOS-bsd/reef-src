// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PreferenceTransformModifier (Environment).

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

public struct _PreferenceTransformModifier<A> where A: SwiftUI.PreferenceKey {
    public var transform: (inout A.Value) -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(key a0: A.Type, transform a1: @escaping (inout A.Value) -> ()) { _skeleton() }
}

extension SwiftUI._PreferenceTransformModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._PreferenceTransformModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
