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
    func write(_ a0: inout SwiftUI.EnvironmentValues) { transform(&a0[keyPath: keyPath]) }
    /// the content (under "transformed") with the value transformed in its environment; the transformed environment is
    /// kept (under "transform") and used again while the input environment and the modifier's bytes stay the same
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] {
        var key = a0.path
        key.append(AnyHashable("transform"))
        let kept = a0.scope.persistentObject(SwiftUI.BodyNode.Key(parts: key)) { SwiftUI.TransformedEnvironment() }
        let same = (kept.modifier as? Self).map { SwiftUI.GraphCompare.bitwiseEqual($0, self) } ?? false
        kept.modifier = self
        let environment: SwiftUI.EnvironmentValues
        if same, let input = kept.input, let output = kept.output, input.isSame(as: a0.environment, customOnly: false) {
            environment = output
        } else {
            var output = a0.environment
            SwiftUI.GraphRule.untracked { write(&output) }
            kept.input = a0.environment
            kept.output = output
            environment = output
        }
        var context = a0
        context.environment = environment
        context.path.append(AnyHashable("transformed"))
        return a1(context)
    }
    public init(keyPath a0: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A>, transform a1: @escaping (inout A) -> ()) {
        keyPath = a0
        transform = a1
    }
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
