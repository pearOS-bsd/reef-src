// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ListDragSession (Gestures).

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

class ListDragSession: NSObject {
    weak var source: SwiftUI.ListDragState?
    let move: ((group: SwiftUI.ListMoveGroup, offsets: [Swift.Int]))? = _skeletonValue()
    let payloads: [Any] = _skeletonValue()
    let token: Swift.String = _skeletonValue()
    static func matching(_ a0: [SwiftUI.DropItem]) -> SwiftUI.ListDragSession? { _skeleton() }
    init(source a0: SwiftUI.ListDragState, move a1: ((group: SwiftUI.ListMoveGroup, offsets: [Swift.Int]))?, payloads a2: [Any]) { _skeleton() }
    static var type: UniformTypeIdentifiers.UTType { _skeleton() }
    static var current: SwiftUI.ListDragSession? { _skeleton() }
    deinit {}
}
