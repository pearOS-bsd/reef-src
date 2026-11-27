// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ToolbarItemPlacement (Navigation).

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

public struct ToolbarItemPlacement {
    let rawValue: Swift.Int
    public static var confirmationAction: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 5) }
    public static var topBarLeading: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 2) }
    public static var automatic: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 0) }
    public static var principal: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 1) }
    public static var navigation: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 2) }
    public static var topBarTrailing: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 3) }
    public static var status: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 4) }
    public static var primaryAction: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 3) }
    public static var cancellationAction: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 6) }
    public static var destructiveAction: SwiftUI.ToolbarItemPlacement { SwiftUI.ToolbarItemPlacement(rawValue: 7) }
}
