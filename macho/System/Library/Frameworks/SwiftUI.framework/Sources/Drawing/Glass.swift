// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Glass (Drawing).

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

public struct Glass {
    let variant: Swift.Int
    var tintColor: SwiftUI.Color?
    var interactive: Swift.Bool
    public static var regular: SwiftUI.Glass { _skeleton() }
    public static var clear: SwiftUI.Glass { _skeleton() }
    public static var identity: SwiftUI.Glass { _skeleton() }
    public func tint(_ a0: SwiftUI.Color?) -> SwiftUI.Glass { _skeleton() }
    public func interactive(_ a0: Swift.Bool) -> SwiftUI.Glass { _skeleton() }
    public static func == (_ a0: SwiftUI.Glass, _ a1: SwiftUI.Glass) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.Glass: Swift.Equatable {
}
