// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollHolder (Lists).

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

class ScrollHolder: NSObject {
    let scrollView: NSScrollView = _skeletonValue()
    let document: SwiftUI.ElementCanvas = _skeletonValue()
    var scrollObservers: [Swift.ObjectIdentifier: () -> ()] = _skeletonValue()
    var targetBehavior: ((behavior: any SwiftUI.ScrollTargetBehavior, environment: SwiftUI.EnvironmentValues, axes: SwiftUI.Axis.Set))? = _skeletonValue()
    var showsVerticalIndicator: Swift.Bool = _skeletonValue()
    var showsHorizontalIndicator: Swift.Bool = _skeletonValue()
    var barInsets: SwiftUI.EdgeInsets = _skeletonValue()
    var edgeFade: (any SwiftUI.FadeLayer)? = _skeletonValue()
    var anchoredSizes: ((document: CGSize, container: CGSize))? = _skeletonValue()
    var keptOrigin: CGPoint = _skeletonValue()
    var resizing: Swift.Bool = _skeletonValue()
    final func updateEdgeFade() { _skeleton() }
    init(host a0: SwiftUI.ViewHost) { _skeleton() }
    final func settle(_ a0: any SwiftUI.ScrollTargetBehavior, environment a1: SwiftUI.EnvironmentValues, axes a2: SwiftUI.Axis.Set) { _skeleton() }
    final func boundsChanged(_ a0: Foundation.Notification) { _skeleton() }
    final func restoreKeptOrigin(container a0: CGSize) { _skeleton() }
    final func scroll(to a0: SwiftUI.Element, anchor a1: SwiftUI.UnitPoint?) { _skeleton() }
    final func applyAnchors(_ a0: SwiftUI.ScrollAnchors, axes a1: SwiftUI.Axis.Set, container a2: CGSize) { _skeleton() }
    deinit {}
}

extension SwiftUI.ScrollHolder: SwiftUI.GraphPresence {
    func leave() { _skeleton() }
}

extension SwiftUI.ScrollHolder: SwiftUI.GraphTeardown {
    func teardown() { _skeleton() }
}
