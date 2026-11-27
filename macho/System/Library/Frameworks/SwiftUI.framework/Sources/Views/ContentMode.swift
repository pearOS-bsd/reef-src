// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentMode (Views).

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

public enum ContentMode {
    case fit
    case fill
    public static var allCases: [SwiftUI.ContentMode] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ContentMode, _ a1: SwiftUI.ContentMode) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ContentMode: Swift.Equatable {
}

extension SwiftUI.ContentMode: Swift.Hashable {
}

extension SwiftUI.ContentMode: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ContentMode]
}
