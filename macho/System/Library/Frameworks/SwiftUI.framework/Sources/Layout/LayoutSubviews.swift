// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LayoutSubviews (Layout).

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

public struct LayoutSubviews {
    let storage: SwiftUI.LayoutChildren?
    let range: Swift.Range<Swift.Int>
    public static func == (_ a0: SwiftUI.LayoutSubviews, _ a1: SwiftUI.LayoutSubviews) -> Swift.Bool { _skeleton() }
    public var startIndex: Swift.Int { _skeleton() }
    public var endIndex: Swift.Int { _skeleton() }
    public subscript(_ a0: Swift.Int) -> SwiftUI.LayoutSubview { _skeleton() }
    public subscript(_ a0: Swift.Range<Swift.Int>) -> SwiftUI.LayoutSubviews { _skeleton() }
    public var layoutDirection: SwiftUI.LayoutDirection { _skeleton() }
}

extension SwiftUI.LayoutSubviews: Swift.Equatable {
}

extension SwiftUI.LayoutSubviews: Swift.RandomAccessCollection {
    public typealias Element = SwiftUI.LayoutSubview
    public typealias Index = Swift.Int
    public typealias SubSequence = SwiftUI.LayoutSubviews
    public typealias Indices = Swift.Range<Swift.Int>
}

extension SwiftUI.LayoutSubviews: Swift.BidirectionalCollection {
}

extension SwiftUI.LayoutSubviews: Swift.Collection {
    public typealias Iterator = Swift.IndexingIterator<SwiftUI.LayoutSubviews>
}

extension SwiftUI.LayoutSubviews: Swift.Sequence {
}
