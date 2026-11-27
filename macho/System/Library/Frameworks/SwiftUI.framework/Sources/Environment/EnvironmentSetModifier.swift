// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentSetModifier (Environment).

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

struct EnvironmentSetModifier {
    let apply: (inout SwiftUI.EnvironmentValues) -> ()
}

extension SwiftUI.EnvironmentSetModifier: SwiftUI.PrimitiveModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] {
        // the content built in a copy of the context whose environment the closure changed
        var context = a0
        apply(&context.environment)
        return a1(context)
    }
}

extension SwiftUI.EnvironmentSetModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
