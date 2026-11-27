// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _EnvironmentKeyTransformModifier (Environment).

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

public struct _EnvironmentKeyTransformModifier<A> {
    public var keyPath: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A>
    public var transform: (inout A) -> ()
    func write(_ a0: inout SwiftUI.EnvironmentValues) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(keyPath a0: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A>, transform a1: @escaping (inout A) -> ()) { _skeleton() }
}

extension SwiftUI._EnvironmentKeyTransformModifier: SwiftUI.EnvironmentWriting {
}

extension SwiftUI._EnvironmentKeyTransformModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._EnvironmentKeyTransformModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._EnvironmentKeyTransformModifier: SwiftUI._SceneModifier {
}
