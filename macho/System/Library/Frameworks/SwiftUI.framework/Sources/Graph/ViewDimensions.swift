// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewDimensions (Graph).

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
    public subscript(_ a0: SwiftUI.VerticalAlignment) -> CoreGraphics.CGFloat { _skeleton() }
    public subscript(explicit a0: SwiftUI.VerticalAlignment) -> CoreGraphics.CGFloat? { _skeleton() }
    public subscript(_ a0: SwiftUI.HorizontalAlignment) -> CoreGraphics.CGFloat { _skeleton() }
    public subscript(explicit a0: SwiftUI.HorizontalAlignment) -> CoreGraphics.CGFloat? { _skeleton() }
}
