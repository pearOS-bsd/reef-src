// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationSizing (Navigation).

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

public protocol PresentationSizing {
    func proposedSize(`for` a0: SwiftUI.PresentationSizingRoot, context a1: SwiftUI.PresentationSizingContext) -> SwiftUI.ProposedViewSize
}

extension SwiftUI.PresentationSizing where Self == SwiftUI.AutomaticPresentationSizing {
    public static var automatic: SwiftUI.AutomaticPresentationSizing { _skeleton() }
}

extension SwiftUI.PresentationSizing where Self == SwiftUI.FittedPresentationSizing {
    public static var fitted: SwiftUI.FittedPresentationSizing { _skeleton() }
}

extension SwiftUI.PresentationSizing where Self == SwiftUI.FormPresentationSizing {
    public static var form: SwiftUI.FormPresentationSizing { _skeleton() }
}

extension SwiftUI.PresentationSizing where Self == SwiftUI.PagePresentationSizing {
    public static var page: SwiftUI.PagePresentationSizing { _skeleton() }
}

extension SwiftUI.PresentationSizing {
    public func fitted(horizontal a0: Swift.Bool, vertical a1: Swift.Bool) -> some SwiftUI.PresentationSizing { self }
    public func sticky(horizontal a0: Swift.Bool, vertical a1: Swift.Bool) -> some SwiftUI.PresentationSizing { self }
}
