// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HostingDriver (AppKitBridging).

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

class HostingDriver {
    let host: SwiftUI.ViewHost = _skeletonValue()
    let canvas: SwiftUI.ElementCanvas = _skeletonValue()
    weak var view: NSView?
    var lastIdeal: CGSize? = _skeletonValue()
    weak var measuredRoot: SwiftUI.Element?
    var refreshedGeneration: Swift.Int = _skeletonValue()
    var lastScheme: SwiftUI.ColorScheme? = _skeletonValue()
    var lastInsets: SwiftUI.EdgeInsets = _skeletonValue()
    var lastDrawn: [SwiftUI.DrawnPart]? = _skeletonValue()
    var lastDrawnSize: CGSize = _skeletonValue()
    weak var placedRoot: SwiftUI.Element?
    var placedBounds: CGRect? = _skeletonValue()
    var placedScale: CoreGraphics.CGFloat = _skeletonValue()
    var appliedColorScheme: SwiftUI.ColorScheme?? = _skeletonValue()
    var appliedMinimizeBehavior: SwiftUI.WindowInteractionBehavior? = _skeletonValue()
    var sizesChangedSincePlaced: Swift.Bool = _skeletonValue()
    var placing: Swift.Bool = _skeletonValue()
    var takingInLayoutChange: Swift.Bool = _skeletonValue()
    var deferredWhileHidden: Swift.Bool = _skeletonValue()
    var dismissAction: SwiftUI.PresentationDismiss? = _skeletonValue()
    var windowObservers: [any NSObjectProtocol] = _skeletonValue()
    weak var observedWindow: NSWindow?
    var responderObserver: SwiftUI.ResponderObserver? = _skeletonValue()
    lazy var dismissHandler: SwiftUI.DismissHandler = _skeletonValue()
    var idealSizeChanged: (() -> ())? = _skeletonValue()
    final func layout() { _skeleton() }
    final func observeClosing(of a0: NSWindow?) { _skeleton() }
    final func environmentChanged() { _skeleton() }
    init(rootType a0: Any.Type) { _skeleton() }
    final func environment() -> SwiftUI.EnvironmentValues { _skeleton() }
    final func markAllDirty(_ a0: SwiftUI.BodyNode) { _skeleton() }
    final func currentRoot() -> SwiftUI.Element? { _skeleton() }
    final func contentChanged() { _skeleton() }
    deinit {}
}

extension SwiftUI.HostingDriver: SwiftUI.ElementCanvasDriver {
    var canvasView: SwiftUI.ElementCanvas? { _skeleton() }
    func measurementChanged() { _skeleton() }
}
