// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSHostingController (AppKitBridging).

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

open class NSHostingController<A>: NSViewController where A: SwiftUI.View {
    var storedRootView: A = _skeletonValue()
    var hostingView: (SwiftUI.NSHostingView<A>)? = _skeletonValue()
    public var sizingOptions: SwiftUI.NSHostingSizingOptions = _skeletonValue()
    public var sceneBridgingOptions: SwiftUI.NSHostingSceneBridgingOptions = _skeletonValue()
    public var safeAreaRegions: SwiftUI.SafeAreaRegions = _skeletonValue()
    public init(rootView a0: A) { _skeleton() }
    open var rootView: A {
        get { _skeleton() }
        set { _skeleton() }
    }
    public required init?(coder a0: NSCoder) { _skeleton() }
    public init?(coder a0: NSCoder, rootView a1: A) { _skeleton() }
    public override init(nibName a0: Swift.String?, bundle a1: Bundle?) { _skeleton() }
    public override func loadView() { _skeleton() }
    open func sizeThatFits(`in` a0: CGSize) -> CGSize { _skeleton() }
    func updatePreferredContentSize() { _skeleton() }
    deinit {}
}
