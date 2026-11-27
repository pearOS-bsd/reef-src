// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextSelection (Text).

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

public struct TextSelection {
    public var indices: SwiftUI.TextSelection.Indices
    public var affinity: SwiftUI.TextSelectionAffinity
    init(ranges a0: [_NSRange], `in` a1: Swift.String) { _skeleton() }
    func ranges(`in` a0: Swift.String) -> [_NSRange] { _skeleton() }
    public init(range a0: Swift.Range<Swift.String.Index>) { _skeleton() }
    public init(ranges a0: Swift.RangeSet<Swift.String.Index>) { _skeleton() }
    public init(insertionPoint a0: Swift.String.Index) { _skeleton() }
    public var isInsertion: Swift.Bool { _skeleton() }
    public static func == (_ a0: SwiftUI.TextSelection, _ a1: SwiftUI.TextSelection) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public enum Indices {
        case selection(Swift.Range<Swift.String.Index>)
        case multiSelection(Swift.RangeSet<Swift.String.Index>)
        public static func == (_ a0: SwiftUI.TextSelection.Indices, _ a1: SwiftUI.TextSelection.Indices) -> Swift.Bool { _skeleton() }
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
    }
}

extension SwiftUI.TextSelection.Indices: Swift.Hashable {
}

extension SwiftUI.TextSelection.Indices: Swift.Equatable {
}

extension SwiftUI.TextSelection: Swift.Hashable {
}

extension SwiftUI.TextSelection: Swift.Equatable {
}
