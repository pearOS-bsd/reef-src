// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewHost (Views).

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

class ViewHost {
    let root: SwiftUI.BodyNode
    var generation: Swift.Int = 0
    var sweptGeneration: Swift.Int = -1
    var needsUpdate: Swift.Bool = true
    var isUpdating: Swift.Bool = false
    var rootElement: SwiftUI.Element? = nil
    var transaction: SwiftUI.Transaction = SwiftUI.Transaction()
    weak var canvas: (any SwiftUI.ElementCanvasDriver)?
    var buildRoot: (SwiftUI.BuildContext) -> [SwiftUI.Element] = { _ in [] }
    var environment: () -> SwiftUI.EnvironmentValues = { SwiftUI.EnvironmentValues() }
    var focus: SwiftUI.FocusTracker = SwiftUI.FocusTracker(host: nil, hadFocus: false, placedInitialFocus: false, handledKeys: [], lastFocused: nil)
    var focusedValues: SwiftUI.FocusedValues = SwiftUI.FocusedValues(values: [:])
    var focusedIdentity: Swift.AnyHashable? = nil
    let focusedValuesSignal: SwiftUI.GraphSignal = SwiftUI.GraphSignal()
    var responderCheckScheduled: Swift.Bool = false
    var scheduled: Swift.Bool = false
    var flushing: Swift.Bool = false
    var actionsScheduled: Swift.Bool = false
    var tornDown: Swift.Bool = false
    var openGroups: [[() -> ()]] = []
    var finishedGroups: [[() -> ()]] = []
    var changeActions: [() -> ()] = []
    var shortcuts: [(SwiftUI.KeyboardShortcut, () -> ())] = []
    var trailingActions: [() -> ()] = []
    let buildRule: SwiftUI.GraphRule = SwiftUI.GraphRule()
    final func scrollTo(_ a0: Swift.AnyHashable, anchor a1: SwiftUI.UnitPoint?, `in` a2: SwiftUI.Element?) { _skeleton() }
    /// the shortcuts registered by this build from index a0 on
    final func shortcuts(since a0: Swift.Int) -> [(SwiftUI.KeyboardShortcut, () -> ())] {
        a0 == 0 ? shortcuts : Array(shortcuts[a0...])
    }
    /// note that the views must be built again (keeping an animated transaction over a plain one) and, once per
    /// round, ask the canvas to update: from the run loop when the window is visible, else from the main queue
    final func setNeedsUpdate(_ a0: SwiftUI.Transaction) {
        needsUpdate = true
        if a0.animation != nil || transaction.animation == nil { transaction = a0 }
        guard !scheduled else { return }
        scheduled = true
        RunLoop.main.perform(inModes: [.common]) { [weak self] in
            guard let self else { return }
            guard let window = self.canvas?.canvasView?.window, window.isVisible, self.scheduled else { return }
            self.flushScheduledUpdate()
        }
        DispatchQueue.main.async { [weak self] in
            guard let self, self.scheduled else { return }
            self.flushScheduledUpdate()
        }
    }
    /// the body of both scheduled closures of setNeedsUpdate
    private final func flushScheduledUpdate() {
        scheduled = false
        flushing = true
        canvas?.contentChanged()
        flushing = false
        runPendingActions()
    }
    init(rootType a0: Any.Type) {
        root = SwiftUI.BodyNode(type: a0, host: nil)
        root.host = self
        buildRule.host = self
        focus.host = self
    }
    /// run what the last builds queued (outside any rule): change actions, finished groups, trailing actions
    final func runPendingActions() {
        guard !isUpdating, openGroups.isEmpty else { return }
        var actions = changeActions
        actions.append(contentsOf: finishedGroups.flatMap { $0 })
        actions.append(contentsOf: trailingActions)
        finishedGroups = []
        changeActions = []
        trailingActions = []
        guard !actions.isEmpty else { return }
        SwiftUI.GraphRule.untracked {
            for action in actions { action() }
        }
    }
    /// queue an action into the open build group (or as a finished group of its own)
    final func enqueue(_ a0: @escaping () -> ()) {
        if openGroups.isEmpty {
            finishedGroups.append([a0])
        } else {
            openGroups[openGroups.count - 1].append(a0)
        }
        scheduleActions()
    }
    final func scheduleActions() {
        guard !flushing, !actionsScheduled else { return }
        actionsScheduled = true
        DispatchQueue.main.async {
            self.actionsScheduled = false
            self.runPendingActions()
        }
    }
    final func performShortcut(_ a0: NSEvent) -> Swift.Bool { _skeleton() }
    /// build the views again when needed: a new generation, the root built under the build rule with a fresh context,
    /// its elements made one root element
    final func update() -> Swift.Bool {
        guard needsUpdate, !isUpdating, let canvas else { return false }
        needsUpdate = false
        isUpdating = true
        tornDown = false
        generation += 1
        root.generation = generation
        let environment = self.environment()
        let context = SwiftUI.BuildContext(host: self, scope: root, path: [], environment: environment, canvas: canvas.canvasView,
            modifierContents: [:], styleLabels: [:], stackAxis: nil, container: .none, scrollAxes: [])
        shortcuts = []
        openGroups.append([])
        let elements = buildRule.track { buildRoot(context) }
        if let group = openGroups.popLast(), !group.isEmpty {
            finishedGroups.insert(group, at: 0)
        }
        let element = SwiftUI.singleElement(elements)
        element.adopt()
        rootElement = element
        isUpdating = false
        transaction = SwiftUI.Transaction()
        return true
    }
    /// after a layout pass: sweep the nodes not visited by this generation, run pending actions while flushing, check
    /// the first responder when asked, sync the focus and publish the focused values
    final func finishLayout() {
        if sweptGeneration != generation {
            sweptGeneration = generation
            root.sweep(generation)
        }
        if flushing { runPendingActions() }
        if responderCheckScheduled {
            responderCheckScheduled = false
            if let canvasView = canvas?.canvasView, let window = canvasView.window {
                focus.responderChanged(window.firstResponder, canvas: canvasView)
            }
        }
        focus.sync(canvas: canvas?.canvasView)
        publishFocusedValues()
    }
    final func publishFocusedValues() { _skeleton() }
    deinit {}
}
