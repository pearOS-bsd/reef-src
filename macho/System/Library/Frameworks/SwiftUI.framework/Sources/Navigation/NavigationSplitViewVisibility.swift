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
    public func encode(to a0: any Swift.Encoder) throws { _skeleton() }
    public init(from a0: any Swift.Decoder) throws { _skeleton() }
    public static var detailOnly: SwiftUI.NavigationSplitViewVisibility { _skeleton() }
    public static var doubleColumn: SwiftUI.NavigationSplitViewVisibility { _skeleton() }
    public static var all: SwiftUI.NavigationSplitViewVisibility { _skeleton() }
    public static var automatic: SwiftUI.NavigationSplitViewVisibility { _skeleton() }
    public static func == (_ a0: SwiftUI.NavigationSplitViewVisibility, _ a1: SwiftUI.NavigationSplitViewVisibility) -> Swift.Bool { _skeleton() }
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
