// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TaskValueModifier (Views).

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

public struct _TaskValueModifier<A> where A: Swift.Equatable {
    var value: A
    var priority: Swift.TaskPriority
    var action: @Sendable () async -> ()
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    public init(value a0: A, priority a1: Swift.TaskPriority, action a2: @escaping @Sendable () async -> ()) { _skeleton() }
}

extension SwiftUI._TaskValueModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._TaskValueModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
