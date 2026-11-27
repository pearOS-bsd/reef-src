// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TraitWritingModifier (Environment).

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

public struct _TraitWritingModifier<A> where A: SwiftUI._ViewTraitKey {
    public let value: A.Value
    var tag: Swift.AnyHashable? { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(value a0: A.Value) { _skeleton() }
}

extension SwiftUI._TraitWritingModifier: SwiftUI.TagWriting {
}

extension SwiftUI._TraitWritingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._TraitWritingModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
