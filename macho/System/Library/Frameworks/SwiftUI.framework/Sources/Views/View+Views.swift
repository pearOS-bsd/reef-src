// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Views area.

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
    public func modifier<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<Self, A1> { SwiftUI.ModifiedContent(content: self, modifier: a0) }
    public static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public func contentMargins(_ a0: CoreGraphics.CGFloat, `for` a1: SwiftUI.ContentMarginPlacement) -> some SwiftUI.View { contentMargins(.all, Optional(a0), for: a1) }
    public func contentMargins(_ a0: SwiftUI.Edge.Set, _ a1: CoreGraphics.CGFloat?, `for` a2: SwiftUI.ContentMarginPlacement) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { guard a2.rawValue != 2 else { return }
            let length = a1 ?? 0
            var margins = $0.scrollContentMargins
            if a0.contains(.top) { margins.top = length }
            if a0.contains(.leading) { margins.leading = length }
            if a0.contains(.bottom) { margins.bottom = length }
            if a0.contains(.trailing) { margins.trailing = length }
            $0[SwiftUI.ScrollContentMarginsKey.self] = margins }) }
    public func onModifierKeysChanged(mask a0: SwiftUI.EventModifiers, initial a1: Swift.Bool, _ a2: @escaping (SwiftUI.EventModifiers, SwiftUI.EventModifiers) -> ()) -> some SwiftUI.View { self }
    public func modifierKeyAlternate<A1>(_ a0: SwiftUI.EventModifiers, _ a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
}
