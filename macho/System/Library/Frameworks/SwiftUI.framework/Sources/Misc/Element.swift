// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Element: a node of the element tree that views build into (layout, drawing, hit testing); the base class of every element.

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
    var frame: CGRect = .zero
    var layoutPriority: Swift.Double = 0
    var zIndex: Swift.Double = 0
    var explicitID: Swift.AnyHashable? = nil
    var isHidden: Swift.Bool = false
    var allowsHitTesting: Swift.Bool = true
    var alignmentGuides: [SwiftUI.AlignmentKey: (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat] = [:]
    var traits: [Swift.ObjectIdentifier: Any] = [:]
    var preferenceOperations: [SwiftUI.PreferenceOperation] = []
    var coordinateSpaceName: Swift.AnyHashable? = nil
    var gridColumns: Swift.Int = 1
    var gridAnchor: SwiftUI.UnitPoint? = nil
    var gridColumnAlignment: SwiftUI.HorizontalAlignment? = nil
    var gridUnsizedAxes: SwiftUI.Axis.Set = []
    weak var hostCanvas: SwiftUI.ElementCanvas?
    var panelChoice: SwiftUI.PanelChoice? = nil
    var sizeCache: [SwiftUI.ProposalKey: CGSize] = [:]
    var flexibleIdealAxes: SwiftUI.Axis.Set = []
    var takesKeyFocus: Swift.Bool = false
    var spacingOverride: SwiftUI.SpacingKind? = nil
    weak var parent: SwiftUI.Element?
    final func sizeThatFits(_ a0: SwiftUI.ProposedViewSize) -> CGSize {
        // an unspecified dimension is cached under -1
        let key = SwiftUI.ProposalKey(width: a0.width ?? -1, height: a0.height ?? -1)
        if let size = sizeCache[key] { return size }
        let size = measure(a0)
        sizeCache[key] = size
        return size
    }
    var children: [SwiftUI.Element] { [] }
    func debugTree(_ a0: Swift.Int) -> Swift.String {
        var line = String(repeating: "  ", count: a0)
        line += _typeName(type(of: self), qualified: false)
        line += " "
        line += "\(frame)"
        if isHidden { line += " hidden" }
        if type(of: self) == SwiftUI.TextElement.self, let text = self as? SwiftUI.TextElement {
            line += " \"" + text.string.string + "\""
        }
        line += debugDetail
        var lines = [line]
        lines.append(contentsOf: children.map { $0.debugTree(a0 + 1) })
        return lines.joined(separator: "\n")
    }
    var debugDetail: Swift.String { "" }
    var hasTextBaseline: Swift.Bool { children.contains { $0.hasTextBaseline } }
    func hasFlexibleIdeal(_ a0: SwiftUI.Axis) -> Swift.Bool {
        flexibleIdealAxes.contains(a0 == .horizontal ? .horizontal : .vertical)
    }
    var isSpacer: Swift.Bool { false }
    func invalidateSizes() {
        sizeCache = [:]
        for child in children { child.invalidateSizes() }
    }
    func liveSizesChanged() -> Swift.Bool { false }
    func refreshSizes() -> Swift.Bool {
        var changed = liveSizesChanged()
        for child in children where child.refreshSizes() { changed = true }
        if !changed { return false }
        sizeCache = [:]
        return true
    }
    func place(`in` a0: CGRect) { frame = a0 }
    var affectsChildDrawing: Swift.Bool { false }
    var drawsOutsideCanvas: Swift.Bool { false }
    func alignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat {
        if let guide = alignmentGuides[a0] {
            var dimensions = SwiftUI.ViewDimensions(width: a1.width, height: a1.height)
            dimensions.element = self
            return guide(dimensions)
        }
        if let value = explicitAlignment(a0, size: a1) { return value }
        return a0.defaultValue(width: a1.width, height: a1.height)
    }
    func adopt() {
        for child in children {
            child.parent = self
            child.adopt()
        }
    }
    func dump(_ a0: Swift.Int, into a1: inout [Swift.String]) {
        let name = String(describing: type(of: self))
        var shape = ""
        if type(of: self) == SwiftUI.ShapeElement.self, let element = self as? SwiftUI.ShapeElement { shape = " " + element.label }
        var line = String(repeating: "  ", count: a0)
        line += name
        line += " "
        line += "\(frame)"
        line += isHidden ? " hidden" : ""
        line += allowsHitTesting ? "" : " nohit"
        line += shape
        a1.append(line)
        if a0 < 40 {
            for child in children { child.dump(a0 + 1, into: &a1) }
        }
    }
    func takesEvents(_ a0: CGPoint) -> Swift.Bool {
        guard allowsHitTesting, !isHidden else { return false }
        if self is any SwiftUI.PressableElement || self is SwiftUI.PlatformViewElement {
            return containsContent(a0) || frame.contains(a0)
        }
        for child in children where child.takesEvents(a0) { return true }
        return false
    }
    func forEach(_ a0: (SwiftUI.Element) -> ()) {
        a0(self)
        for child in children { child.forEach(a0) }
    }
    var drawsOwnContent: Swift.Bool { false }
    var drawOverflow: CoreGraphics.CGFloat { 0 }
    func drawsSame(`as` a0: SwiftUI.Element) -> Swift.Bool { a0 === self }
    func draw(_ a0: SwiftUI.RenderContext) {
        for child in children where !child.isHidden { child.draw(a0) }
    }
    var spacingKind: SwiftUI.SpacingKind { .other }
    func explicitAlignment(_ a0: SwiftUI.AlignmentKey, size a1: CGSize) -> CoreGraphics.CGFloat? { nil }
    func hitTest(_ a0: CGPoint) -> SwiftUI.Element? {
        guard allowsHitTesting, !isHidden else { return nil }
        // the topmost (last) child first
        for child in children.reversed() {
            if let hit = child.hitTest(a0) { return hit }
        }
        return nil
    }
    func containsContent(_ a0: CGPoint) -> Swift.Bool {
        guard allowsHitTesting, !isHidden else { return false }
        return children.contains { $0.containsContent(a0) }
    }
    func measure(_ a0: SwiftUI.ProposedViewSize) -> CGSize { .zero }
    final func adoptGridPlacement(of a0: SwiftUI.Element) {
        gridColumns = a0.gridColumns
        gridAnchor = a0.gridAnchor
        gridColumnAlignment = a0.gridColumnAlignment
        gridUnsizedAxes = a0.gridUnsizedAxes
    }
    init() {}
    static var liveSizeGeneration: Swift.Int = 0
    deinit {}
}
