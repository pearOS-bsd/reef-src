// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentedWindowRoot (Navigation).

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

struct PresentedWindowRoot<A, B> where A: Swift.Hashable, B: SwiftUI.View {
    let presented: SwiftUI.PresentedValue
    let content: (SwiftUI.Binding<A?>) -> B
    var _value: SwiftUI.State<A?>
    init(presented a0: SwiftUI.PresentedValue, content a1: @escaping (SwiftUI.Binding<A?>) -> B, defaultValue a2: (() -> A)?) { _skeleton() }
    var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.PresentedWindowRoot: SwiftUI.View {
}
