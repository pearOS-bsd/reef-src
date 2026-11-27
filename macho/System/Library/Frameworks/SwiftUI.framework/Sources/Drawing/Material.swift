// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Material (Drawing).

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

public struct Material {
    let thickness: Swift.Int
    public static var regular: SwiftUI.Material { _skeleton() }
    public static var thick: SwiftUI.Material { _skeleton() }
    public static var thin: SwiftUI.Material { _skeleton() }
    public static var ultraThin: SwiftUI.Material { _skeleton() }
    public static var ultraThick: SwiftUI.Material { _skeleton() }
    public static var bar: SwiftUI.Material { _skeleton() }
}

extension SwiftUI.Material: SwiftUI.ShapeStyle {
    public typealias Resolved = Swift.Never
}
