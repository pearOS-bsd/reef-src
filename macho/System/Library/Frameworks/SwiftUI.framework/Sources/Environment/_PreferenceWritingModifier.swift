// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PreferenceWritingModifier (Environment).

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

public struct _PreferenceWritingModifier<A> where A: SwiftUI.PreferenceKey {
    public var value: A.Value
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(key a0: A.Type, value a1: A.Value) { _skeleton() }
}

extension SwiftUI._PreferenceWritingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._PreferenceWritingModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
