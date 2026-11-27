// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Element (Misc).

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

class Element {
    var frame: CGRect = _skeletonValue()
    var layoutPriority: Swift.Double = _skeletonValue()
    var zIndex: Swift.Double = _skeletonValue()
    var explicitID: Swift.AnyHashable? = _skeletonValue()
    var isHidden: Swift.Bool = _skeletonValue()
    var allowsHitTesting: Swift.Bool = _skeletonValue()
    var alignmentGuides: [SwiftUI.AlignmentKey: (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat] = _skeletonValue()
    var traits: [Swift.ObjectIdentifier: Any] = _skeletonValue()
    var preferenceOperations: [SwiftUI.PreferenceOperation] = _skeletonValue()
    var coordinateSpaceName: Swift.AnyHashable? = _skeletonValue()
    var gridColumns: Swift.Int = _skeletonValue()
    var gridAnchor: SwiftUI.UnitPoint? = _skeletonValue()
    var gridColumnAlignment: SwiftUI.HorizontalAlignment? = _skeletonValue()
    var gridUnsizedAxes: SwiftUI.Axis.Set = _skeletonValue()
    weak var hostCanvas: SwiftUI.ElementCanvas?
    var panelChoice: SwiftUI.PanelChoice? = _skeletonValue()
    var sizeCache: [SwiftUI.ProposalKey: CGSize] = _skeletonValue()
    var flexibleIdealAxes: SwiftUI.Axis.Set = _skeletonValue()
    var takesKeyFocus: Swift.Bool = _skeletonValue()
    var spacingOverride: SwiftUI.SpacingKind? = _skeletonValue()
    weak var parent: SwiftUI.Element?
    final func sizeThatFits(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    var children: [SwiftUI.Element] { _skeleton() }
    func debugTree(_ a0: Swift.Int) -> Swift.String { _skeleton() }
    var debugDetail: Swift.String { _skeleton() }
    var hasTextBaseline: Swift.Bool { _skeleton() }
    func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool { _skeleton() }
    var isSpacer: Swift.Bool { _skeleton() }
    func invalidateSizes() { _skeleton() }
    func liveSizesChanged() -> Swift.Bool { _skeleton() }
    func refreshSizes() -> Swift.Bool { _skeleton() }
    func place(`in` a0: CGRect) { _skeleton() }
    var affectsChildDrawing: Swift.Bool { _skeleton() }
    var drawsOutsideCanvas: Swift.Bool { _skeleton() }
    func alignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat { _skeleton() }
    func adopt() { _skeleton() }
    func dump(_ a0: Swift.Int, into a1: inout [Swift.String]) { _skeleton() }
    func takesEvents(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func forEach(_ a0: (SwiftUI.Element) -> ()) { _skeleton() }
    var drawsOwnContent: Swift.Bool { _skeleton() }
    var drawOverflow: CoreGraphics.CGFloat { _skeleton() }
    func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
    func draw(_ a0: SwiftUI.RenderContext) { _skeleton() }
    var spacingKind: SwiftUI.SpacingKind { _skeleton() }
    func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { _skeleton() }
    func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    func containsContent(_ a0: CGPoint) -> Swift.Bool { _skeleton() }
    func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { _skeleton() }
    final func adoptGridPlacement(of a0: SwiftUI.Element) { _skeleton() }
    init() { _skeleton() }
    static var liveSizeGeneration: Swift.Int { _skeleton() }
    deinit {}
}
