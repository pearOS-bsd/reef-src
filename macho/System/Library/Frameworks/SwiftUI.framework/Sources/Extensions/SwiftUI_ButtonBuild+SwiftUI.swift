// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_ButtonBuild.

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

enum ButtonBuild {
    static func build<A>(label a0: A, action a1: @escaping () -> (), context a2: SwiftUI.BuildContext) -> [SwiftUI.Element] where A: SwiftUI.View { _skeleton() }
    static func isBuiltin(_ a0: Any) -> Swift.Bool { _skeleton() }
    static func kind(_ a0: Any) -> SwiftUI.BuiltinButtonKind { _skeleton() }
    static func builtin(_ a0: SwiftUI.BuiltinButtonKind, labelContent a1: SwiftUI.ModifierContent, interaction a2: SwiftUI.ButtonInteraction, context a3: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}
