// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewBuilder (Views).

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

/// The result builder of view content. Apple's interface emits its single-view, empty, optional and expression forms into
/// the clients; they are written here the same way (no symbols), for the views built inside the module.
@resultBuilder public struct ViewBuilder {
    @_alwaysEmitIntoClient public static func buildExpression<C: SwiftUI.View>(_ content: C) -> C { content }
    @_alwaysEmitIntoClient public static func buildBlock() -> SwiftUI.EmptyView { SwiftUI.EmptyView() }
    @_alwaysEmitIntoClient public static func buildBlock<C: SwiftUI.View>(_ content: C) -> C { content }
    @_alwaysEmitIntoClient public static func buildIf<C: SwiftUI.View>(_ content: C?) -> C? { content }
    @_alwaysEmitIntoClient public static func buildLimitedAvailability<C: SwiftUI.View>(_ content: C) -> SwiftUI.AnyView { SwiftUI.AnyView(content) }
    static func buildBlock<each A>(_ a0: repeat each A) -> SwiftUI.TupleView<(repeat each A)> where repeat each A: SwiftUI.View { SwiftUI.TupleView((repeat each a0)) }
    static func buildEither<A, B>(first a0: A) -> SwiftUI._ConditionalContent<A, B> where A: SwiftUI.View, B: SwiftUI.View { SwiftUI._ConditionalContent(storage: .trueContent(a0)) }
    static func buildEither<A, B>(second a0: B) -> SwiftUI._ConditionalContent<A, B> where A: SwiftUI.View, B: SwiftUI.View { SwiftUI._ConditionalContent(storage: .falseContent(a0)) }
}
