// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProgressViewStyle (Drawing).

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

public protocol ProgressViewStyle {
    associatedtype Body: SwiftUI.View
    func makeBody(configuration a0: SwiftUI.ProgressViewStyleConfiguration) -> Self.Body
}

extension SwiftUI.ProgressViewStyle where Self == SwiftUI.LinearProgressViewStyle {
    public static var linear: SwiftUI.LinearProgressViewStyle { _skeleton() }
}

extension SwiftUI.ProgressViewStyle where Self == SwiftUI.DefaultProgressViewStyle {
    public static var automatic: SwiftUI.DefaultProgressViewStyle { _skeleton() }
}

extension SwiftUI.ProgressViewStyle where Self == SwiftUI.CircularProgressViewStyle {
    public static var circular: SwiftUI.CircularProgressViewStyle { _skeleton() }
}
