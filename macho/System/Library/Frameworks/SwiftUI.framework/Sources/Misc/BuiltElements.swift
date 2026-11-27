// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BuiltElements (Misc).

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

/// the elements a body node built last time, with what they were built in and the fields of each element as they were
/// then (a reused build puts them back)
class BuiltElements {
    let elements: [SwiftUI.Element]
    var generation: Swift.Int
    let environment: SwiftUI.EnvironmentValues
    let shortcuts: [(SwiftUI.KeyboardShortcut, () -> ())]
    weak var canvas: SwiftUI.ElementCanvas?
    let hasCanvas: Swift.Bool
    let stackAxis: SwiftUI.Axis?
    let container: SwiftUI.Container
    let scrollAxes: SwiftUI.Axis.Set
    let fields: [SwiftUI.ElementFields]
    /// whether context a0 builds the same way: no modifier contents or style labels in it, the same canvas, stack axis,
    /// container and scroll axes
    final func matches(_ a0: SwiftUI.BuildContext) -> Swift.Bool {
        guard a0.modifierContents.isEmpty, a0.styleLabels.isEmpty else { return false }
        guard canvas === a0.canvas, hasCanvas == (a0.canvas != nil) else { return false }
        return stackAxis == a0.stackAxis && container == a0.container && scrollAxes == a0.scrollAxes
    }
    /// reused in generation a0: the fields of the elements as they were built
    final func restore(_ a0: Swift.Int) {
        generation = a0
        for (element, saved) in zip(elements, fields) {
            element.layoutPriority = saved.layoutPriority
            element.zIndex = saved.zIndex
            element.explicitID = saved.explicitID
            element.isHidden = saved.isHidden
            element.allowsHitTesting = saved.allowsHitTesting
            element.alignmentGuides = saved.alignmentGuides
            element.traits = saved.traits
            element.preferenceOperations = saved.preferenceOperations
            element.coordinateSpaceName = saved.coordinateSpaceName
            element.gridColumns = saved.gridColumns
            element.gridAnchor = saved.gridAnchor
            element.gridColumnAlignment = saved.gridColumnAlignment
            element.gridUnsizedAxes = saved.gridUnsizedAxes
            element.panelChoice = saved.panelChoice
            element.flexibleIdealAxes = saved.flexibleIdealAxes
            element.takesKeyFocus = saved.takesKeyFocus
            element.spacingOverride = saved.spacingOverride
        }
    }
    init(elements a0: [SwiftUI.Element], context a1: SwiftUI.BuildContext, shortcuts a2: [(SwiftUI.KeyboardShortcut, () -> ())]) {
        elements = a0
        generation = a1.host.generation
        environment = a1.environment
        shortcuts = a2
        canvas = a1.canvas
        hasCanvas = a1.canvas != nil
        stackAxis = a1.stackAxis
        container = a1.container
        scrollAxes = a1.scrollAxes
        fields = a0.map { element in
            SwiftUI.ElementFields(layoutPriority: element.layoutPriority, zIndex: element.zIndex, explicitID: element.explicitID,
                isHidden: element.isHidden, allowsHitTesting: element.allowsHitTesting, alignmentGuides: element.alignmentGuides,
                traits: element.traits, preferenceOperations: element.preferenceOperations,
                coordinateSpaceName: element.coordinateSpaceName, gridColumns: element.gridColumns, gridAnchor: element.gridAnchor,
                gridColumnAlignment: element.gridColumnAlignment, gridUnsizedAxes: element.gridUnsizedAxes,
                panelChoice: element.panelChoice, flexibleIdealAxes: element.flexibleIdealAxes, takesKeyFocus: element.takesKeyFocus,
                spacingOverride: element.spacingOverride)
        }
    }
    deinit {}
}
