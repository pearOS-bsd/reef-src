// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// VisualEffectModifier (Drawing).

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

struct VisualEffectModifier {
    let effect: (SwiftUI.EmptyVisualEffect, SwiftUI.GeometryProxy) -> any SwiftUI.VisualEffect
    var _geometry: SwiftUI.State<SwiftUI.VisualEffectGeometry>
    func body(content a0: SwiftUI._ViewModifier_Content<SwiftUI.VisualEffectModifier>) -> some SwiftUI.View { _skeleton() }
}

extension SwiftUI.VisualEffectModifier: SwiftUI.ViewModifier {
}
