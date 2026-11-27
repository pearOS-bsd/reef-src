// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ForEachSubviewCollection (Lists).

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

public struct ForEachSubviewCollection<A> where A: SwiftUI.View {
    let source: (SwiftUI.BuildContext) -> [SwiftUI.Subview]
    var subviews: [SwiftUI.Subview]
    public var startIndex: Swift.Int { _skeleton() }
    public var endIndex: Swift.Int { _skeleton() }
    public subscript(_ a0: Swift.Int) -> SwiftUI.Subview { _skeleton() }
    init(source a0: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Subview], subviews a1: [SwiftUI.Subview]) { _skeleton() }
    func resolved(`in` a0: SwiftUI.BuildContext) -> SwiftUI.ForEachSubviewCollection<A> { _skeleton() }
}

extension SwiftUI.ForEachSubviewCollection: Swift.RandomAccessCollection {
    public typealias Element = SwiftUI.Subview
    public typealias Index = Swift.Int
    public typealias SubSequence = Swift.Slice<SwiftUI.ForEachSubviewCollection<A>>
    public typealias Indices = Swift.Range<Swift.Int>
}

extension SwiftUI.ForEachSubviewCollection: Swift.BidirectionalCollection {
}

extension SwiftUI.ForEachSubviewCollection: Swift.Collection {
    public typealias Iterator = Swift.IndexingIterator<SwiftUI.ForEachSubviewCollection<A>>
}

extension SwiftUI.ForEachSubviewCollection: Swift.Sequence {
}

extension SwiftUI.ForEachSubviewCollection: SwiftUI.BuildResolvedCollection {
}
