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

public struct ViewBuilder {
    static func buildBlock<each A>(_ a0: repeat each A) -> SwiftUI.TupleView<(repeat each A)> where repeat each A: SwiftUI.View { _skeleton() }
    static func buildEither<A, B>(first a0: A) -> SwiftUI._ConditionalContent<A, B> where A: SwiftUI.View, B: SwiftUI.View { _skeleton() }
    static func buildEither<A, B>(second a0: B) -> SwiftUI._ConditionalContent<A, B> where A: SwiftUI.View, B: SwiftUI.View { _skeleton() }
}
