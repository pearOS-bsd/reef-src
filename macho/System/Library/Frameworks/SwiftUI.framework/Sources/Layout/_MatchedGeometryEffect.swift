// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _MatchedGeometryEffect (Layout).

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

public struct _MatchedGeometryEffect<A> where A: Swift.Hashable {
    var id: A
    var namespace: SwiftUI.Namespace.ID
    var properties: SwiftUI.MatchedGeometryProperties
    var anchor: SwiftUI.UnitPoint
    var isSource: Swift.Bool
    init(id a0: A, namespace a1: SwiftUI.Namespace.ID, properties a2: SwiftUI.MatchedGeometryProperties, anchor a3: SwiftUI.UnitPoint, isSource a4: Swift.Bool) { _skeleton() }
}

extension SwiftUI._MatchedGeometryEffect: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._MatchedGeometryEffect: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
