// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ResponderObserver (AppKitBridging).

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

class ResponderObserver: NSObject {
    weak var window: NSWindow?
    weak var host: SwiftUI.ViewHost?
    init(window a0: NSWindow, host a1: SwiftUI.ViewHost) { _skeleton() }
    deinit {}
}
