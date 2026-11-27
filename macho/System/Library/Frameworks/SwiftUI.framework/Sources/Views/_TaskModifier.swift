// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TaskModifier (Views).

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

public struct _TaskModifier {
    public var action: @Sendable () async -> ()
    public var priority: Swift.TaskPriority
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(priority a0: Swift.TaskPriority, action a1: @escaping @Sendable () async -> ()) { _skeleton() }
}

extension SwiftUI._TaskModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._TaskModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
