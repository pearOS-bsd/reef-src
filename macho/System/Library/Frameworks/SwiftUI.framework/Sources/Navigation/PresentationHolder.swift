// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationHolder (Navigation).

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

class PresentationHolder {
    var environment: SwiftUI.EnvironmentValues = _skeletonValue()
    var anchorFrame: CGRect = _skeletonValue()
    var title: Swift.String? = _skeletonValue()
    var subtitle: Swift.String? = _skeletonValue()
    weak var anchor: SwiftUI.PresentationAnchorElement?
    var sheet: NSWindow? = _skeletonValue()
    weak var sheetHost: NSWindow?
    var popover: NSPopover? = _skeletonValue()
    var alertShown: Swift.Bool = _skeletonValue()
    var panelShown: Swift.Bool = _skeletonValue()
    var helpTag: Swift.Int? = _skeletonValue()
    weak var helpCanvas: NSView?
    var panelDelegate: NSObject? = _skeletonValue()
    var onDismiss: (() -> ())? = _skeletonValue()
    final func update(_ a0: SwiftUI.PresentationModifier.Kind, anchor a1: SwiftUI.PresentationAnchorElement) { _skeleton() }
    final func showOpenPanel(_ a0: SwiftUI.Binding<Swift.Bool>, types a1: [UniformTypeIdentifiers.UTType], multiple a2: Swift.Bool, completion a3: @escaping (Swift.Result<[Foundation.URL], any Swift.Error>) -> ()) { _skeleton() }
    final var window: NSWindow? { _skeleton() }
    final func applyDialogEnvironment(_ a0: NSSavePanel) { _skeleton() }
    final func teardown() { _skeleton() }
    init() { _skeleton() }
    deinit {}
}

extension SwiftUI.PresentationHolder: SwiftUI.GraphTeardown {
}
