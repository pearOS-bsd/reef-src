// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewDimensions: the size of a view and its alignment guides, as seen by an alignment guide closure.

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

public struct ViewDimensions {
    public var width: CoreGraphics.CGFloat
    public var height: CoreGraphics.CGFloat
    weak var element: SwiftUI.Element?
    public subscript(_ a0: SwiftUI.VerticalAlignment) -> CoreGraphics.CGFloat { value(a0.key) }
    public subscript(explicit a0: SwiftUI.VerticalAlignment) -> CoreGraphics.CGFloat? { explicitValue(a0.key) }
    public subscript(_ a0: SwiftUI.HorizontalAlignment) -> CoreGraphics.CGFloat { value(a0.key) }
    public subscript(explicit a0: SwiftUI.HorizontalAlignment) -> CoreGraphics.CGFloat? { explicitValue(a0.key) }
    // (one body for both axes in the binary: ...HorizontalAlignmentVcigTm)
    private func value(_ key: SwiftUI.AlignmentKey) -> CoreGraphics.CGFloat {
        if let element = element { return element.alignment(key, size: CGSize(width: width, height: height)) }
        return key.defaultValue(width: width, height: height)
    }
    private func explicitValue(_ key: SwiftUI.AlignmentKey) -> CoreGraphics.CGFloat? {
        guard let element = element else { return nil }
        let size = CGSize(width: width, height: height)
        if element.alignmentGuides[key] != nil { return element.alignment(key, size: size) }
        return element.explicitAlignment(key, size: size)
    }
}
