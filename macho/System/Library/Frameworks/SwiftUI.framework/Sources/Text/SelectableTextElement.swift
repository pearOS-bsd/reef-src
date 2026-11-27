// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SelectableTextElement (Text).

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

class SelectableTextElement: SwiftUI.WrapperElement {
    let text: SwiftUI.TextElement = _skeletonValue()
    let state: SwiftUI.TextSelectionState = _skeletonValue()
    var drawnSelection: [Swift.Int]? = _skeletonValue()
    var layout: ((width: CoreGraphics.CGFloat, manager: NSLayoutManager, container: NSTextContainer, storage: NSTextStorage))? = _skeletonValue()
    final func unit(at a0: Swift.Int, clicks a1: Swift.Int) -> _NSRange { _skeleton() }
    init(_ a0: SwiftUI.TextElement, state a1: SwiftUI.TextSelectionState) { _skeleton() }
    final var selects: Swift.Bool { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    override func takesEvents(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    override var drawsOwnContent: Swift.Bool { _skeleton() }
    final var shownSelection: [Swift.Int]? { _skeleton() }
    override func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    final func rects(`for` a0: _NSRange) -> [CGRect] { _skeleton() }
    final func laidOut() -> (origin: CGPoint, manager: NSLayoutManager, container: NSTextContainer) { _skeleton() }
    static func word(`in` a0: NSString, at a1: Swift.Int) -> _NSRange { _skeleton() }
    deinit {}
}

extension SwiftUI.SelectableTextElement: SwiftUI.CursorElement {
    func cursor(at a0: CGPoint) -> NSCursor? { _skeleton() }
    var cursor: NSCursor { _skeleton() }
}

extension SwiftUI.SelectableTextElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
