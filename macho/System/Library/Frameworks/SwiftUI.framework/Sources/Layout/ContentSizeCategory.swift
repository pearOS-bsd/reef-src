// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentSizeCategory (Layout).

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

public enum ContentSizeCategory {
    case extraSmall
    case small
    case medium
    case large
    case extraLarge
    case extraExtraLarge
    case extraExtraExtraLarge
    case accessibilityMedium
    case accessibilityLarge
    case accessibilityExtraLarge
    case accessibilityExtraExtraLarge
    case accessibilityExtraExtraExtraLarge
    public var isAccessibilityCategory: Swift.Bool { _skeleton() }
    public static var allCases: [SwiftUI.ContentSizeCategory] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ContentSizeCategory, _ a1: SwiftUI.ContentSizeCategory) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ContentSizeCategory: Swift.Equatable {
}

extension SwiftUI.ContentSizeCategory: Swift.Hashable {
}

extension SwiftUI.ContentSizeCategory: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ContentSizeCategory]
}
