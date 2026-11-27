// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TagTraitWritingModifier (Environment).

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

public struct _TagTraitWritingModifier<A> where A: Swift.Hashable {
    public var value: A
    public var includeOptional: Swift.Bool
    var tag: Swift.AnyHashable? { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(value a0: A, includeOptional a1: Swift.Bool) { _skeleton() }
}

extension SwiftUI._TagTraitWritingModifier: SwiftUI.TagWriting {
}

extension SwiftUI._TagTraitWritingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._TagTraitWritingModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
