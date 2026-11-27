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

class BuiltElements {
    let elements: [SwiftUI.Element] = _skeletonValue()
    var generation: Swift.Int = _skeletonValue()
    let environment: SwiftUI.EnvironmentValues = _skeletonValue()
    let shortcuts: [(SwiftUI.KeyboardShortcut, () -> ())] = _skeletonValue()
    weak var canvas: SwiftUI.ElementCanvas?
    let hasCanvas: Swift.Bool = _skeletonValue()
    let stackAxis: SwiftUI.Axis? = _skeletonValue()
    let container: SwiftUI.Container = _skeletonValue()
    let scrollAxes: SwiftUI.Axis.Set = _skeletonValue()
    let fields: [SwiftUI.ElementFields] = _skeletonValue()
    final func matches(_ a0: SwiftUI.BuildContext) -> Swift.Bool { _skeleton() }
    final func restore(_ a0: Swift.Int) { _skeleton() }
    init(elements a0: [SwiftUI.Element], context a1: SwiftUI.BuildContext, shortcuts a2: [(SwiftUI.KeyboardShortcut, () -> ())]) { _skeleton() }
    deinit {}
}
