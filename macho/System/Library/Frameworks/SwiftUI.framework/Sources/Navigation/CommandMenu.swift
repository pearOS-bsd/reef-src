// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CommandMenu (Navigation).

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

public struct CommandMenu<A> where A: SwiftUI.View {
    let name: SwiftUI.Text
    let content: A
    var menuName: SwiftUI.Text { _skeleton() }
    var menuEntries: [SwiftUI.MenuEntry] { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.Text, content a1: () -> A) { _skeleton() }
    public init<A1>(_ a0: A1, content a1: () -> A) where A1: Swift.StringProtocol { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.CommandMenu: SwiftUI.CommandMenuParts {
}

extension SwiftUI.CommandMenu: SwiftUI.Commands {
    public typealias Body = Swift.Never
}
