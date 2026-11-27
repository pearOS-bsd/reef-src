// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SubviewsCollectionSlice (Misc).

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

public struct SubviewsCollectionSlice {
    let subviews: Swift.ArraySlice<SwiftUI.Subview>
    public var startIndex: Swift.Int { _skeleton() }
    public var endIndex: Swift.Int { _skeleton() }
    public subscript(_ a0: Swift.Int) -> SwiftUI.Subview { _skeleton() }
    public subscript(_ a0: Swift.Range<Swift.Int>) -> SwiftUI.SubviewsCollectionSlice { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.SubviewsCollectionSlice: Swift.RandomAccessCollection {
    public typealias Element = SwiftUI.Subview
    public typealias Index = Swift.Int
    public typealias SubSequence = SwiftUI.SubviewsCollectionSlice
    public typealias Indices = Swift.Range<Swift.Int>
}

extension SwiftUI.SubviewsCollectionSlice: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.SubviewsCollectionSlice: Swift.BidirectionalCollection {
}

extension SwiftUI.SubviewsCollectionSlice: Swift.Collection {
    public typealias Iterator = Swift.IndexingIterator<SwiftUI.SubviewsCollectionSlice>
}

extension SwiftUI.SubviewsCollectionSlice: Swift.Sequence {
}

extension SwiftUI.SubviewsCollectionSlice: SwiftUI.PrimitiveView {
}
