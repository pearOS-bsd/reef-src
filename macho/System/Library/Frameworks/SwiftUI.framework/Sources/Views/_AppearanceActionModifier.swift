// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _AppearanceActionModifier (Views).

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

public struct _AppearanceActionModifier {
    public var appear: (() -> ())?
    public var disappear: (() -> ())?
    public init(appear a0: (() -> ())?, disappear a1: (() -> ())?) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._AppearanceActionModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._AppearanceActionModifier: SwiftUI.PrimitiveModifier {
}
