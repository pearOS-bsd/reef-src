// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollDismissesKeyboardMode (Controls).

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

public struct ScrollDismissesKeyboardMode {
    let rawValue: Swift.Int
    public static var automatic: SwiftUI.ScrollDismissesKeyboardMode { SwiftUI.ScrollDismissesKeyboardMode(rawValue: 0) }
    public static var immediately: SwiftUI.ScrollDismissesKeyboardMode { SwiftUI.ScrollDismissesKeyboardMode(rawValue: 1) }
    public static var interactively: SwiftUI.ScrollDismissesKeyboardMode { SwiftUI.ScrollDismissesKeyboardMode(rawValue: 2) }
    public static var never: SwiftUI.ScrollDismissesKeyboardMode { SwiftUI.ScrollDismissesKeyboardMode(rawValue: 3) }
}
