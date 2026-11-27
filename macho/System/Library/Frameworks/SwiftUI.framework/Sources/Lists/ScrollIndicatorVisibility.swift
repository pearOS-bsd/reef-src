// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollIndicatorVisibility (Lists).

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

public struct ScrollIndicatorVisibility {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ScrollIndicatorVisibility { SwiftUI.ScrollIndicatorVisibility(rawValue: 0) }
    public static var visible: SwiftUI.ScrollIndicatorVisibility { SwiftUI.ScrollIndicatorVisibility(rawValue: 1) }
    public static var hidden: SwiftUI.ScrollIndicatorVisibility { SwiftUI.ScrollIndicatorVisibility(rawValue: 2) }
    public static var never: SwiftUI.ScrollIndicatorVisibility { SwiftUI.ScrollIndicatorVisibility(rawValue: 3) }
    public static func == (_ a0: SwiftUI.ScrollIndicatorVisibility, _ a1: SwiftUI.ScrollIndicatorVisibility) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.ScrollIndicatorVisibility: Swift.Equatable {
}
