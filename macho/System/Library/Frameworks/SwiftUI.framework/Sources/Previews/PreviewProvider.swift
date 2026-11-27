// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreviewProvider (Previews).

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

public protocol PreviewProvider: SwiftUI._PreviewProvider {
    associatedtype Previews: SwiftUI.View
    static var platform: SwiftUI.PreviewPlatform? { get }
    static var previews: Self.Previews { get }
}

extension SwiftUI.PreviewProvider {
    public static var platform: SwiftUI.PreviewPlatform? { _skeleton() }
    public static var _previews: Any { _skeleton() }
    public static var _platform: SwiftUI.PreviewPlatform? { _skeleton() }
}
