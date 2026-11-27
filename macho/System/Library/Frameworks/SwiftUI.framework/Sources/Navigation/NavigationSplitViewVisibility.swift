// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationSplitViewVisibility (Navigation).

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

public struct NavigationSplitViewVisibility {
    let rawValue: Swift.Int
    init(rawValue a0: Swift.Int) { rawValue = a0 }
    public func encode(to a0: any Swift.Encoder) throws { _skeleton() }
    public init(from a0: any Swift.Decoder) throws { _skeleton() }
    public static var detailOnly: SwiftUI.NavigationSplitViewVisibility { SwiftUI.NavigationSplitViewVisibility(rawValue: 0) }
    public static var doubleColumn: SwiftUI.NavigationSplitViewVisibility { SwiftUI.NavigationSplitViewVisibility(rawValue: 1) }
    public static var all: SwiftUI.NavigationSplitViewVisibility { SwiftUI.NavigationSplitViewVisibility(rawValue: 2) }
    public static var automatic: SwiftUI.NavigationSplitViewVisibility { SwiftUI.NavigationSplitViewVisibility(rawValue: 3) }
    public static func == (_ a0: SwiftUI.NavigationSplitViewVisibility, _ a1: SwiftUI.NavigationSplitViewVisibility) -> Swift.Bool { a0.rawValue == a1.rawValue }
    enum CodingKeys {
        case rawValue
    }
}

extension SwiftUI.NavigationSplitViewVisibility: Swift.Equatable {
}

extension SwiftUI.NavigationSplitViewVisibility: Swift.Decodable {
}

extension SwiftUI.NavigationSplitViewVisibility: Swift.Encodable {
}

extension SwiftUI.NavigationSplitViewVisibility.CodingKeys: Swift.CustomDebugStringConvertible {
}

extension SwiftUI.NavigationSplitViewVisibility.CodingKeys: Swift.CustomStringConvertible {
}

extension SwiftUI.NavigationSplitViewVisibility.CodingKeys: Swift.Hashable {
}

extension SwiftUI.NavigationSplitViewVisibility.CodingKeys: Swift.Equatable {
}

extension SwiftUI.NavigationSplitViewVisibility.CodingKeys: Swift.CodingKey {
}
