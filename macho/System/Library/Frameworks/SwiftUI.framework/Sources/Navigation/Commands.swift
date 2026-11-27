// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Commands (Navigation).

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

public protocol Commands {
    associatedtype Body: SwiftUI.Commands
    static func _makeCommands(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._CommandsInputs) -> SwiftUI._CommandsOutputs
    func _resolve(into a0: inout SwiftUI._ResolvedCommands)
    var body: Self.Body { get }
}

extension SwiftUI.Commands {
    public static func _makeCommands(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._CommandsInputs) -> SwiftUI._CommandsOutputs { _skeleton() }
    public func _resolve(into a0: inout SwiftUI._ResolvedCommands) { _skeleton() }
}
