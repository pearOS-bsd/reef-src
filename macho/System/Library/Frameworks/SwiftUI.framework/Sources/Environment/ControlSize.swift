// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlSize (Environment).

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

public enum ControlSize {
    case mini
    case small
    case regular
    case large
    case extraLarge
    public static var allCases: [SwiftUI.ControlSize] { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.ControlSize, _ a1: SwiftUI.ControlSize) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ControlSize: Swift.Equatable {
}

extension SwiftUI.ControlSize: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.ControlSize]
}

extension SwiftUI.ControlSize: Swift.Hashable {
}
