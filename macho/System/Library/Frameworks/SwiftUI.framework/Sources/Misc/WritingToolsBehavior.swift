// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WritingToolsBehavior (Misc).

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

public struct WritingToolsBehavior {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.WritingToolsBehavior { SwiftUI.WritingToolsBehavior(rawValue: 0) }
    public static var complete: SwiftUI.WritingToolsBehavior { SwiftUI.WritingToolsBehavior(rawValue: 1) }
    public static var limited: SwiftUI.WritingToolsBehavior { SwiftUI.WritingToolsBehavior(rawValue: 2) }
    public static var disabled: SwiftUI.WritingToolsBehavior { SwiftUI.WritingToolsBehavior(rawValue: 3) }
    public var hashValue: Swift.Int { _hashValue(for: self) }
    public static func == (_ a0: SwiftUI.WritingToolsBehavior, _ a1: SwiftUI.WritingToolsBehavior) -> Swift.Bool { a0.rawValue == a1.rawValue }
    public func hash(into a0: inout Swift.Hasher) { a0.combine(rawValue) }
}

extension SwiftUI.WritingToolsBehavior: Swift.Hashable {
}

extension SwiftUI.WritingToolsBehavior: Swift.Equatable {
}
