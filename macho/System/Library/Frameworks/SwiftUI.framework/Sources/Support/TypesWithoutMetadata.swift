// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Helpers of the reconstruction (stub bodies).

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

public enum PreviewLayout {}
/// what a dynamic property installs itself with: the body node that owns the view and the property's slot
/// (the binary has one more word between them, not known yet)
struct PropertyInstallContext {
    var node: SwiftUI.BodyNode
    var index: Swift.Int
}
struct RenderContext {}
enum TransferExport {}
protocol FadeLayer {}
