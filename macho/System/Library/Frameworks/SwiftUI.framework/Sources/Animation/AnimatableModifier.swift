// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimatableModifier (Animation).

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

public protocol AnimatableModifier: SwiftUI.Animatable, SwiftUI.ViewModifier {
}

extension SwiftUI.AnimatableModifier {
    public static func _makeView(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewListOutputs { _skeleton() }
}
