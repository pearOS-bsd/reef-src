// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyView (Views).

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

public struct AnyView {
    public var storage: SwiftUI.AnyViewStorageBase
    public init<A>(_ a0: A) where A: SwiftUI.View { storage = SwiftUI.AnyViewStorage(a0) }
    public init<A>(erasing a0: A) where A: SwiftUI.View { self.init(a0) }
}

extension SwiftUI.AnyView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.AnyView: SwiftUI.PrimitiveView {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { storage.buildElements(a0) }
}

extension SwiftUI.AnyView: SwiftUI.MenuContributor {
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
}
