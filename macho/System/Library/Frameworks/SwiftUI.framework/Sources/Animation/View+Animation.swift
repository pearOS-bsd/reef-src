// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Animation area.

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

extension SwiftUI.View {
    public func animation<A1>(_ a0: SwiftUI.Animation?, value a1: A1) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func contentTransition(_ a0: SwiftUI.ContentTransition) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.contentTransition, value: a0)) }
    public func keyframeAnimator<A1, B1, C1, D1>(initialValue a0: A1, trigger a1: B1, content a2: @escaping @Sendable (SwiftUI.PlaceholderContentView<Self>, A1) -> C1, keyframes a3: @escaping (A1) -> D1) -> some SwiftUI.View where A1 == D1.Value, B1: Swift.Equatable, C1: SwiftUI.View, D1: SwiftUI.Keyframes { self }
    public func keyframeAnimator<A1, B1, C1>(initialValue a0: A1, repeating a1: Swift.Bool, content a2: @escaping @Sendable (SwiftUI.PlaceholderContentView<Self>, A1) -> B1, keyframes a3: @escaping (A1) -> C1) -> some SwiftUI.View where A1 == C1.Value, B1: SwiftUI.View, C1: SwiftUI.Keyframes { self }
    public func transition(_ a0: SwiftUI.AnyTransition) -> some SwiftUI.View { self }
    public func transition<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.Transition { self }
    public func animation(_ a0: SwiftUI.Animation?) -> some SwiftUI.View { self }
    public func transaction(_ a0: @escaping (inout SwiftUI.Transaction) -> ()) -> some SwiftUI.View { self }
    public func transaction<A1>(value a0: A1, _ a1: @escaping (inout SwiftUI.Transaction) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func transaction<A1>(_ a0: @escaping (inout SwiftUI.Transaction) -> (), body a1: (SwiftUI.PlaceholderContentView<Self>) -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func animation<A1>(_ a0: SwiftUI.Animation?, body a1: (SwiftUI.PlaceholderContentView<Self>) -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
}
