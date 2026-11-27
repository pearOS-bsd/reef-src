// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TextFieldCoordinator (AppKitBridging).

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

final class TextFieldCoordinator: NSObject {
    var binding: (SwiftUI.Binding<Swift.String>)? = _skeletonValue()
    var onCommit: () -> () = _skeletonValue()
    var submit: [() -> ()] = _skeletonValue()
    weak var host: SwiftUI.ViewHost?
    weak var field: NSTextField?
    var applyingSelection: Swift.Bool = _skeletonValue()
    var selectionObserver: (any NSObjectProtocol)? = _skeletonValue()
    var selection: (SwiftUI.Binding<SwiftUI.TextSelection?>)? = _skeletonValue()
    func observeSelection() { _skeleton() }
    func selectionChanged() { _skeleton() }
    func applySelection(text a0: Swift.String) { _skeleton() }
    func controlTextDidBeginEditing(_ a0: Foundation.Notification) { _skeleton() }
    func controlTextDidChange(_ a0: Foundation.Notification) { _skeleton() }
    override init() { _skeleton() }
    deinit {}
}
