// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ControlStylesKey (Drawing).

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

enum ControlStylesKey {
    static var defaultValue: [Swift.ObjectIdentifier: Any] { [:] }
}

extension SwiftUI.ControlStylesKey: SwiftUI.EnvironmentKey {
    typealias Value = [Swift.ObjectIdentifier: Any]
}
