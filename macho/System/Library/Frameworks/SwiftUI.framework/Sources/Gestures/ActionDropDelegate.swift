// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ActionDropDelegate (Gestures).

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

struct ActionDropDelegate {
    let types: [UniformTypeIdentifiers.UTType]
    let isTargeted: (SwiftUI.Binding<Swift.Bool>)?
    let action: ([NSItemProvider], CGPoint) -> Swift.Bool
}

extension SwiftUI.ActionDropDelegate: SwiftUI.DropDelegate {
    func performDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool { _skeleton() }
}
