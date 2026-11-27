// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WindowStyle (Drawing).

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

public protocol WindowStyle {
}

extension SwiftUI.WindowStyle where Self == SwiftUI.DefaultWindowStyle {
    public static var automatic: SwiftUI.DefaultWindowStyle { _skeleton() }
}

extension SwiftUI.WindowStyle where Self == SwiftUI.HiddenTitleBarWindowStyle {
    public static var hiddenTitleBar: SwiftUI.HiddenTitleBarWindowStyle { _skeleton() }
}

extension SwiftUI.WindowStyle where Self == SwiftUI.TitleBarWindowStyle {
    public static var titleBar: SwiftUI.TitleBarWindowStyle { _skeleton() }
}
