// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Images area.

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

extension SwiftUI.View {
    public func symbolRenderingMode(_ a0: SwiftUI.SymbolRenderingMode?) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.SymbolRenderingModeKey.self] = a0 }) }
    public func imageScale(_ a0: SwiftUI.Image.Scale) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.imageScale, value: a0)) }
    public func symbolVariant(_ a0: SwiftUI.SymbolVariants) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.SymbolVariantsKey.self] = a0 }) }
    public func symbolEffectsRemoved(_ a0: Swift.Bool) -> some SwiftUI.View { self }
}
