// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TaskModifier2 (Views).

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

public struct _TaskModifier2 {
    let name: Swift.String
    let priority: Swift.TaskPriority
    let action: () async -> ()
    public init(name a0: Swift.String, executorPreference a1: (any Swift.TaskExecutor)?, priority a2: Swift.TaskPriority, action a3: __owned @escaping @isolated(any) () async -> ()) { _skeleton() }
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI._TaskModifier2: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._TaskModifier2: SwiftUI.PrimitiveModifier {
}
