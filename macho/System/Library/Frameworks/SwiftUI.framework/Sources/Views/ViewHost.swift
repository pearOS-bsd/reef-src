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
    let root: SwiftUI.BodyNode = _skeletonValue()
    var generation: Swift.Int = _skeletonValue()
    var sweptGeneration: Swift.Int = _skeletonValue()
    var needsUpdate: Swift.Bool = _skeletonValue()
    var isUpdating: Swift.Bool = _skeletonValue()
    var rootElement: SwiftUI.Element? = _skeletonValue()
    var transaction: SwiftUI.Transaction = _skeletonValue()
    weak var canvas: (any SwiftUI.ElementCanvasDriver)?
    var buildRoot: (SwiftUI.BuildContext) -> [SwiftUI.Element] = _skeletonValue()
    var environment: () -> SwiftUI.EnvironmentValues = _skeletonValue()
    var focus: SwiftUI.FocusTracker = _skeletonValue()
    var focusedValues: SwiftUI.FocusedValues = _skeletonValue()
    var focusedIdentity: Swift.AnyHashable? = _skeletonValue()
    let focusedValuesSignal: SwiftUI.GraphSignal = _skeletonValue()
    var responderCheckScheduled: Swift.Bool = _skeletonValue()
    var scheduled: Swift.Bool = _skeletonValue()
    var flushing: Swift.Bool = _skeletonValue()
    var actionsScheduled: Swift.Bool = _skeletonValue()
    var tornDown: Swift.Bool = _skeletonValue()
    var openGroups: [[() -> ()]] = _skeletonValue()
    var finishedGroups: [[() -> ()]] = _skeletonValue()
    var changeActions: [() -> ()] = _skeletonValue()
    var shortcuts: [(SwiftUI.KeyboardShortcut, () -> ())] = _skeletonValue()
    var trailingActions: [() -> ()] = _skeletonValue()
    let buildRule: SwiftUI.GraphRule = _skeletonValue()
    final func scrollTo(_ a0: Swift.AnyHashable, anchor a1: SwiftUI.UnitPoint?, `in` a2: SwiftUI.Element?) { _skeleton() }
    final func shortcuts(since a0: Swift.Int) -> [(SwiftUI.KeyboardShortcut, () -> ())] { _skeleton() }
    final func setNeedsUpdate(_ a0: SwiftUI.Transaction) { _skeleton() }
    init(rootType a0: Any.Type) { _skeleton() }
    final func runPendingActions() { _skeleton() }
    final func enqueue(_ a0: @escaping () -> ()) { _skeleton() }
    final func scheduleActions() { _skeleton() }
    final func performShortcut(_ a0: NSEvent) -> Swift.Bool { _skeleton() }
    final func update() -> Swift.Bool { _skeleton() }
    final func finishLayout() { _skeleton() }
    final func publishFocusedValues() { _skeleton() }
    deinit {}
}
