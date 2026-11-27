// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ModifierKeyAlternateView (Views).

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

struct ModifierKeyAlternateView<A, B> where A: SwiftUI.View, B: SwiftUI.View {
    let modifiers: SwiftUI.EventModifiers
    let base: A
    let alternate: B
    var _held: SwiftUI.State<Swift.Bool>
    var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.ModifierKeyAlternateView: SwiftUI.View {
}
