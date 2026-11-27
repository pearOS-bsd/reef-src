// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListDropDelegate (Gestures).

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

struct ListDropDelegate {
    let list: SwiftUI.ListContentElement
    func dropUpdated(info a0: SwiftUI.DropInfo) -> SwiftUI.DropProposal? { _skeleton() }
    func dropExited(info a0: SwiftUI.DropInfo) { _skeleton() }
    func performDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ListDropDelegate: SwiftUI.DropDelegate {
}
