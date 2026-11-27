// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TaskValueModifier2 (Views).

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

public struct _TaskValueModifier2<A> where A: Swift.Equatable {
    let id: A
    let name: Swift.String
    let priority: Swift.TaskPriority
    let action: () async -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(id a0: A, name a1: Swift.String, executorPreference a2: (any Swift.TaskExecutor)?, priority a3: Swift.TaskPriority, action a4: __owned @escaping @isolated(any) () async -> ()) { _skeleton() }
}

extension SwiftUI._TaskValueModifier2: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._TaskValueModifier2: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
