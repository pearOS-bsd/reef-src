// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_TextFieldBuild.

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

enum TextFieldBuild {
    static func custom<A, B>(_ a0: A, field a1: SwiftUI.TextField<B>, context a2: SwiftUI.BuildContext) -> [SwiftUI.Element] where A: SwiftUI.TextFieldStyle, B: SwiftUI.View { _skeleton() }
    static func isBuiltIn(_ a0: any SwiftUI.TextFieldStyle) -> Swift.Bool { _skeleton() }
}
