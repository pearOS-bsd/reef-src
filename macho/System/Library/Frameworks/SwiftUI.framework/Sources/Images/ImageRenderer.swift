// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ImageRenderer (Images).

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

open class ImageRenderer<A> where A: SwiftUI.View {
    public var content: A = _skeletonValue()
    public var scale: CoreGraphics.CGFloat = _skeletonValue()
    public init(content a0: A) { _skeleton() }
    public final var nsImage: NSImage? { _skeleton() }
    public final var cgImage: CGImage? { _skeleton() }
    deinit {}
}

extension SwiftUI.ImageRenderer: Combine.ObservableObject {
    public typealias ObjectWillChangePublisher = Combine.ObservableObjectPublisher
}
