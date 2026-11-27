// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CommandGroup (Navigation).

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

public struct CommandGroup<A> where A: SwiftUI.View {
    let placement: SwiftUI.CommandGroupPlacement
    let position: SwiftUI.CommandGroup<A>.Position
    let content: A
    var positionValue: Swift.Int { _skeleton() }
    var menuEntries: [SwiftUI.MenuEntry] { _skeleton() }
    public init(before a0: SwiftUI.CommandGroupPlacement, addition a1: () -> A) { _skeleton() }
    public init(after a0: SwiftUI.CommandGroupPlacement, addition a1: () -> A) { _skeleton() }
    public init(replacing a0: SwiftUI.CommandGroupPlacement, addition a1: () -> A) { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    enum Position {
        case before
        case after
        case replacing
        static func == (_ a0: SwiftUI.CommandGroup<A>.Position, _ a1: SwiftUI.CommandGroup<A>.Position) -> Swift.Bool { _skeleton() }
        func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
        var hashValue: Swift.Int { _hashValue(for: self) }
    }
}

extension SwiftUI.CommandGroup: SwiftUI.CommandGroupParts {
    var placementName: Swift.String { _skeleton() }
}

extension SwiftUI.CommandGroup: SwiftUI.Commands {
    public typealias Body = Swift.Never
}

extension SwiftUI.CommandGroup.Position: Swift.Hashable {
}

extension SwiftUI.CommandGroup.Position: Swift.Equatable {
}
