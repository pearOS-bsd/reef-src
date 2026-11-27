// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Visibility (Misc).

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

@frozen public enum Visibility {
    case automatic
    case visible
    case hidden
    public static func == (_ a0: SwiftUI.Visibility, _ a1: SwiftUI.Visibility) -> Swift.Bool { _skeleton() }
    public static var allCases: [SwiftUI.Visibility] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.Visibility: Swift.Equatable {
}

extension SwiftUI.Visibility: Swift.Hashable {
}

extension SwiftUI.Visibility: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.Visibility]
}
