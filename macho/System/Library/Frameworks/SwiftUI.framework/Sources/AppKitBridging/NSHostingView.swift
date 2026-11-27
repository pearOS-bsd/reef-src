// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSHostingView (AppKitBridging).

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

open class NSHostingView<A>: NSView where A: SwiftUI.View {
    public internal(set) var rootView: A = _skeletonValue()
    public internal(set) var sizingOptions: SwiftUI.NSHostingSizingOptions = _skeletonValue()
    public internal(set) var safeAreaRegions: SwiftUI.SafeAreaRegions = _skeletonValue()
    public internal(set) var sceneBridgingOptions: SwiftUI.NSHostingSceneBridgingOptions = _skeletonValue()
    let driver: SwiftUI.HostingDriver = _skeletonValue()
    var isUpdatingWindowSize: Swift.Bool = _skeletonValue()
    var sizeConstraints: [NSLayoutConstraint] = _skeletonValue()
    public init(rootView a0: A) { _skeleton() }
    var dismissAction: SwiftUI.PresentationDismiss? {
        get { _skeleton() }
        set { _skeleton() }
    }
    func lowerContentPriorities() { _skeleton() }
    public required init?(coder a0: NSCoder) { _skeleton() }
    public init?(coder a0: NSCoder, rootView a1: A) { _skeleton() }
    public override init(frame a0: CGRect) { _skeleton() }
    public override var isFlipped: Swift.Bool { _skeleton() }
    public override var isOpaque: Swift.Bool { _skeleton() }
    public override var intrinsicContentSize: CGSize { _skeleton() }
    final func withSafeArea(_ a0: CGSize) -> CGSize { _skeleton() }
    public override var fittingSize: CGSize { _skeleton() }
    open func sizeThatFits(`in` a0: CGSize) -> CGSize { _skeleton() }
    var idealContentSize: CGSize { _skeleton() }
    var idealSizeChanged: (() -> ())? {
        get { _skeleton() }
        set { _skeleton() }
    }
    func _panelChoices() -> [NSDictionary] { _skeleton() }
    func _minimumLayoutSize() -> CGSize { _skeleton() }
    func _maximumLayoutSize() -> CGSize { _skeleton() }
    public override func hitTest(_ a0: CGPoint) -> NSView? { _skeleton() }
    public override func validateProposedFirstResponder(_ a0: NSResponder, `for` a1: NSEvent?) -> Swift.Bool { _skeleton() }
    public override func layout() { _skeleton() }
    public override func setFrameSize(_ a0: CGSize) { _skeleton() }
    public override func performKeyEquivalent(with a0: NSEvent) -> Swift.Bool { _skeleton() }
    public override func viewDidMoveToWindow() { _skeleton() }
    func updateSizeConstraints() { _skeleton() }
    func updateWindowSizeLimits() { _skeleton() }
    public override func viewDidUnhide() { _skeleton() }
    public override func viewDidChangeEffectiveAppearance() { _skeleton() }
    final var hostedSafeAreaInsets: SwiftUI.EdgeInsets { _skeleton() }
    final var popoverContentSize: CGSize { _skeleton() }
    deinit {}
}

extension SwiftUI.NSHostingView: SwiftUI.HostingWindowSizing {
}

extension SwiftUI.NSHostingView: SwiftUI.HostingSafeArea {
}
