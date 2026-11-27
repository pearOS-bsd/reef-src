// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PlatformViewElement (AppKitBridging).

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

class PlatformViewElement: SwiftUI.Element {
    let view: NSView = _skeletonValue()
    weak var canvas: SwiftUI.ElementCanvas?
    let sizing: (SwiftUI.ProposedViewSize) -> CGSize = _skeletonValue()
    var measuredVersion: Swift.Int? = _skeletonValue()
    var attachesAfterActions: Swift.Bool = _skeletonValue()
    weak var host: SwiftUI.ViewHost?
    var isRepresented: Swift.Bool = _skeletonValue()
    override func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    override func place(`in` a0: CGRect) { _skeleton() }
    override var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    override func liveSizesChanged() -> Swift.Bool { _skeleton() }
    override func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    override func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    override var drawsOutsideCanvas: Swift.Bool { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    init(view a0: NSView, canvas a1: SwiftUI.ElementCanvas?, sizing a2: @escaping (SwiftUI.ProposedViewSize) -> CGSize) { _skeleton() }
    func viewFrame(`for` a0: CGRect) -> CGRect { _skeleton() }
    static func measurementChanged(_ a0: NSView) { _skeleton() }
    override func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    static var measuring: Swift.Set<Swift.ObjectIdentifier> { _skeleton() }
    static var versions: NSMapTable<NSView, NSNumber> { _skeleton() }
    deinit {}
}
