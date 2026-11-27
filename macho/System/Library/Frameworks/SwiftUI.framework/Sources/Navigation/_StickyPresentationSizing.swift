// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _StickyPresentationSizing (Navigation).

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

public struct _StickyPresentationSizing<A> where A: SwiftUI.PresentationSizing {
    let base: A
    public func proposedSize(`for` a0: SwiftUI.PresentationSizingRoot, context a1: SwiftUI.PresentationSizingContext) -> SwiftUI.ProposedViewSize { _skeleton() }
    init(base a0: A) { _skeleton() }
}

extension SwiftUI._StickyPresentationSizing: SwiftUI.PresentationSizing {
}
