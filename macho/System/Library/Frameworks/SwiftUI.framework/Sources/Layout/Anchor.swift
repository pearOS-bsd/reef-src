// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Anchor (Layout).

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

public struct Anchor<A> {
    public let box: SwiftUI.AnchorBoxBase
    init(box a0: SwiftUI.AnchorBoxBase) { _skeleton() }
    public struct Source {
        public var box: SwiftUI.AnchorBoxBase
        func resolve(_ a0: CGRect) -> SwiftUI.Anchor<A> { _skeleton() }
    }
}

extension SwiftUI.Anchor.Source where A == CGRect {
    public static var bounds: SwiftUI.Anchor<CGRect>.Source { _skeleton() }
    public static func rect(_ a0: CGRect) -> SwiftUI.Anchor<CGRect>.Source { _skeleton() }
}

extension SwiftUI.Anchor where A: Swift.Equatable {
    public static func == (_ a0: SwiftUI.Anchor<A>, _ a1: SwiftUI.Anchor<A>) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Anchor.Source where A == CGPoint {
    public static func point(_ a0: CGPoint) -> SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static func unitPoint(_ a0: SwiftUI.UnitPoint) -> SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var topLeading: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var top: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var topTrailing: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var leading: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var center: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var trailing: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var bottomLeading: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var bottom: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
    public static var bottomTrailing: SwiftUI.Anchor<CGPoint>.Source { _skeleton() }
}

extension SwiftUI.Anchor: Swift.Equatable where A: Swift.Equatable {
}
