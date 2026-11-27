// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SidebarRowSize (Layout).

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

public enum SidebarRowSize {
    case small
    case medium
    case large
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.SidebarRowSize, _ a1: SwiftUI.SidebarRowSize) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.SidebarRowSize: Swift.Equatable {
}

extension SwiftUI.SidebarRowSize: Swift.Hashable {
}
