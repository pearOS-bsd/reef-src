// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FittedPresentationSizing (Navigation).

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

public struct _FittedPresentationSizing<A> where A: SwiftUI.PresentationSizing {
    let base: A
    let horizontal: Swift.Bool
    let vertical: Swift.Bool
    public func proposedSize(`for` a0: SwiftUI.PresentationSizingRoot, context a1: SwiftUI.PresentationSizingContext) -> SwiftUI.ProposedViewSize { _skeleton() }
    init(base a0: A, horizontal a1: Swift.Bool, vertical a2: Swift.Bool) { _skeleton() }
}

extension SwiftUI._FittedPresentationSizing: SwiftUI.PresentationSizing {
}
