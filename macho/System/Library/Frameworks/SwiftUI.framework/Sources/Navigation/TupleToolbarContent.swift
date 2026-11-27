// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleToolbarContent (Navigation).

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

public struct TupleToolbarContent<A> {
    let value: A
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI.TupleToolbarContent: SwiftUI.CustomizableToolbarContent {
}

extension SwiftUI.TupleToolbarContent: SwiftUI.ToolbarContent {
    public typealias Body = Swift.Never
}

extension SwiftUI.TupleToolbarContent: SwiftUI.ToolbarActionSource {
}
