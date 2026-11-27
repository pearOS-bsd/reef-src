// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// HostingSheetRepresentation (AppKitBridging).

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

extension NSWindow {
    public class HostingSheetRepresentation<A>: NSObject where A: SwiftUI.View {
        let sheet: NSWindow = _skeletonValue()
        weak var parent: NSWindow?
        init(sheet a0: NSWindow, parent a1: NSWindow) { _skeleton() }
        public final func dismiss() { _skeleton() }
        public override init() { _skeleton() }
        deinit {}
    }
}
