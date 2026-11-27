// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationBarItem (Navigation).

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

public struct NavigationBarItem {
    public enum TitleDisplayMode {
        case automatic
        case inline
        case large
        public func hash(into a0: inout Swift.Hasher) { _skeleton() }
        public var hashValue: Swift.Int { _skeleton() }
        public static func == (_ a0: SwiftUI.NavigationBarItem.TitleDisplayMode, _ a1: SwiftUI.NavigationBarItem.TitleDisplayMode) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.NavigationBarItem.TitleDisplayMode: Swift.Equatable {
}

extension SwiftUI.NavigationBarItem.TitleDisplayMode: Swift.Hashable {
}
