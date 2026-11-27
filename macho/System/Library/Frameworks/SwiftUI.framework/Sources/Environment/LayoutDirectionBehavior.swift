// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// LayoutDirectionBehavior (Environment).

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

public enum LayoutDirectionBehavior {
    case mirrors(in: SwiftUI.LayoutDirection)
    case fixed
    public static var mirrors: SwiftUI.LayoutDirectionBehavior { _skeleton() }
    public static func == (_ a0: SwiftUI.LayoutDirectionBehavior, _ a1: SwiftUI.LayoutDirectionBehavior) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.LayoutDirectionBehavior: Swift.Hashable {
}

extension SwiftUI.LayoutDirectionBehavior: Swift.Equatable {
}
