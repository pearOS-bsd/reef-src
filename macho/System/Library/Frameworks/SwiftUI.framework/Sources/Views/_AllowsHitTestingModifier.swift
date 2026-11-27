// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _AllowsHitTestingModifier (Views).

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

public struct _AllowsHitTestingModifier {
    public var allowsHitTesting: Swift.Bool
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(allowsHitTesting a0: Swift.Bool) { _skeleton() }
    public static func == (_ a0: SwiftUI._AllowsHitTestingModifier, _ a1: SwiftUI._AllowsHitTestingModifier) -> Swift.Bool { _skeleton() }
}

extension SwiftUI._AllowsHitTestingModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._AllowsHitTestingModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._AllowsHitTestingModifier: Swift.Equatable {
}
