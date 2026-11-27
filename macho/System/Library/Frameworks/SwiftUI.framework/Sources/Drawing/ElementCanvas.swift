// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ElementCanvas (Drawing).

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

final class ElementCanvas: NSView {
    weak var host: SwiftUI.ViewHost?
    var root: SwiftUI.Element? = _skeletonValue()
    var isRoot: Swift.Bool = _skeletonValue()
    var registeredDropTypes: Swift.Set<Swift.String> = _skeletonValue()
    var dropTarget: SwiftUI.DropElement? = _skeletonValue()
    var liftedLayers: [SwiftUI.ElementCanvas.LiftedLayer] = _skeletonValue()
    var tracking: NSTrackingArea? = _skeletonValue()
    var pressed: (any SwiftUI.PressTarget)? = _skeletonValue()
    var hovered: [Swift.ObjectIdentifier: SwiftUI.HoverState] = _skeletonValue()
    var continuousHovered: [Swift.ObjectIdentifier: SwiftUI.ContinuousHoverState] = _skeletonValue()
    var hoverPoint: CGPoint? = _skeletonValue()
    var contentCursor: NSCursor? = _skeletonValue()
    weak var textSelection: SwiftUI.TextSelectionState?
    var holdsSelectableList: Swift.Bool = _skeletonValue()
    func updateDropTypes() { _skeleton() }
    func leaveDropTarget(_ a0: CGPoint, items a1: [SwiftUI.DropItem]) { _skeleton() }
    func dragMoved(_ a0: any NSDraggingInfo) -> NSDragOperation { _skeleton() }
    override func performDragOperation(_ a0: any NSDraggingInfo) -> Swift.Bool { _skeleton() }
    func press(`for` a0: any SwiftUI.PressableElement, at a1: CGPoint) -> any SwiftUI.PressTarget { _skeleton() }
    func subviewMeasurementChanged(_ a0: NSView) { _skeleton() }
    func restackPlatformViews() { _skeleton() }
    static func collectPlatformViews(_ a0: SwiftUI.Element, into a1: inout [NSView]) { _skeleton() }
    func releaseContent() { _skeleton() }
    func positionLiftedLayers() { _skeleton() }
    override func didAddSubview(_ a0: NSView) { _skeleton() }
    override var needsDisplay: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    func repaintLiftedLayers() { _skeleton() }
    func drawsUnderSubviews(`in` a0: CGRect) -> Swift.Bool { _skeleton() }
    func drawnRect(`in` a0: CGRect) -> CGRect { _skeleton() }
    override func hitTest(_ a0: CGPoint) -> NSView? { _skeleton() }
    override var acceptsFirstResponder: Swift.Bool { _skeleton() }
    override var canBecomeKeyView: Swift.Bool { _skeleton() }
    static func containsTextInput(_ a0: NSView) -> Swift.Bool { _skeleton() }
    override func updateTrackingAreas() { _skeleton() }
    func pressKeepsFocus(at a0: CGPoint) -> Swift.Bool { _skeleton() }
    override func validateProposedFirstResponder(_ a0: NSResponder, `for` a1: NSEvent?) -> Swift.Bool { _skeleton() }
    override func mouseDown(with a0: NSEvent) { _skeleton() }
    override func mouseDragged(with a0: NSEvent) { _skeleton() }
    override func mouseUp(with a0: NSEvent) { _skeleton() }
    override func rightMouseDown(with a0: NSEvent) { _skeleton() }
    func updateHover(_ a0: CGPoint?) { _skeleton() }
    func forEachOwnElement(_ a0: SwiftUI.Element, _ a1: (SwiftUI.Element) -> ()) { _skeleton() }
    override func keyDown(with a0: NSEvent) { _skeleton() }
    func handleKeyEventAroundResponder(_ a0: NSEvent) -> Swift.Bool { _skeleton() }
    override func cancelOperation(_ a0: Any?) { _skeleton() }
    override func resignFirstResponder() -> Swift.Bool { _skeleton() }
    override func responds(to a0: ObjectiveC.Selector?) -> Swift.Bool { _skeleton() }
    func copy(_ a0: Any?) { _skeleton() }
    override func selectAll(_ a0: Any?) { _skeleton() }
    func validateMenuItem(_ a0: NSMenuItem) -> Swift.Bool { _skeleton() }
    override init(frame a0: CGRect) { _skeleton() }
    required init?(coder a0: NSCoder) { _skeleton() }
    func dropItems(_ a0: any NSDraggingInfo) -> [SwiftUI.DropItem] { _skeleton() }
    override func draw(_ a0: CGRect) { _skeleton() }
    static var skipsDrawing: Swift.Bool { _skeleton() }
    deinit {}
    class LiftedLayer {
        let canvas: SwiftUI.ElementCanvas = _skeletonValue()
        let elements: SwiftUI.LiftedElements = _skeletonValue()
        weak var anchor: NSView?
        var drawn: [SwiftUI.DrawnPart]? = _skeletonValue()
        var drawnSize: CGSize = _skeletonValue()
        deinit {}
    }
}
