// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarContent (Navigation).

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

public protocol ToolbarContent {
    associatedtype Body: SwiftUI.ToolbarContent
    static func _makeToolbar(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ToolbarInputs) -> SwiftUI._ToolbarOutputs
    static func _makeContent(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._GraphInputs, resolved a2: inout SwiftUI._ToolbarItemList)
    var body: Self.Body { get }
}

extension SwiftUI.ToolbarContent {
    public func sharedBackgroundVisibility(_ a0: SwiftUI.Visibility) -> some SwiftUI.ToolbarContent { self }
    public static func _makeToolbar(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ToolbarInputs) -> SwiftUI._ToolbarOutputs { _skeleton() }
    public static func _makeContent(content a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._GraphInputs, resolved a2: inout SwiftUI._ToolbarItemList) { _skeleton() }
}

extension SwiftUI.ToolbarContent where Self.Body == Swift.Never {
    public var body: Swift.Never { _skeleton() }
}
