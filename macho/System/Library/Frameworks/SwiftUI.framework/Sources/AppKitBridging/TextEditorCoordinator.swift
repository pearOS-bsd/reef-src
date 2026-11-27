// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextEditorCoordinator (AppKitBridging).

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

final class TextEditorCoordinator: NSObject {
    var binding: (SwiftUI.Binding<Swift.String>)? = _skeletonValue()
    func textDidChange(_ a0: Foundation.Notification) { _skeleton() }
    deinit {}
}
