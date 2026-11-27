// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// UnavailableActionsLayout (Layout).

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

struct UnavailableActionsLayout {
}

extension SwiftUI.UnavailableActionsLayout: SwiftUI.Animatable {
    typealias AnimatableData = SwiftUI.EmptyAnimatableData
}

extension SwiftUI.UnavailableActionsLayout: SwiftUI.Layout {
    typealias Cache = ()
    func sizeThatFits(proposal a0: SwiftUI.ProposedViewSize, subviews a1: SwiftUI.LayoutSubviews, cache a2: inout Self.Cache) -> CGSize { _skeleton() }
    func placeSubviews(`in` a0: CGRect, proposal a1: SwiftUI.ProposedViewSize, subviews a2: SwiftUI.LayoutSubviews, cache a3: inout Self.Cache) { _skeleton() }
}
