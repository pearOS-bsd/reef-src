// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreviewPlatform (Previews).

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

public enum PreviewPlatform {
    case iOS
    case macOS
    case tvOS
    case watchOS
    case visionOS
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.PreviewPlatform, _ a1: SwiftUI.PreviewPlatform) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.PreviewPlatform: Swift.Equatable {
}

extension SwiftUI.PreviewPlatform: Swift.Hashable {
}
