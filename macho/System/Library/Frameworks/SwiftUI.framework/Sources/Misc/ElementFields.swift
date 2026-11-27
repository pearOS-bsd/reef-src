// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ElementFields (Misc).

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

struct ElementFields {
    let layoutPriority: Swift.Double
    let zIndex: Swift.Double
    let explicitID: Swift.AnyHashable?
    let isHidden: Swift.Bool
    let allowsHitTesting: Swift.Bool
    let alignmentGuides: [SwiftUI.AlignmentKey: (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat]
    let traits: [Swift.ObjectIdentifier: Any]
    let preferenceOperations: [SwiftUI.PreferenceOperation]
    let coordinateSpaceName: Swift.AnyHashable?
    let gridColumns: Swift.Int
    let gridAnchor: SwiftUI.UnitPoint?
    let gridColumnAlignment: SwiftUI.HorizontalAlignment?
    let gridUnsizedAxes: SwiftUI.Axis.Set
    let panelChoice: SwiftUI.PanelChoice?
    let flexibleIdealAxes: SwiftUI.Axis.Set
    let takesKeyFocus: Swift.Bool
    let spacingOverride: SwiftUI.SpacingKind?
}
