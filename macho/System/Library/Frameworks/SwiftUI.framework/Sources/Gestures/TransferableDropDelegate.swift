// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TransferableDropDelegate (Gestures).

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

struct TransferableDropDelegate<A> where A: CoreTransferable.Transferable {
    let action: ([A], CGPoint) -> Swift.Bool
    let isTargeted: (Swift.Bool) -> ()
    func performDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool { _skeleton() }
    static func load(_ a0: NSItemProvider) async -> A? { _skeleton() }
    func dropEntered(info a0: SwiftUI.DropInfo) { _skeleton() }
    func dropExited(info a0: SwiftUI.DropInfo) { _skeleton() }
    init(action a0: @escaping ([A], CGPoint) -> Swift.Bool, isTargeted a1: @escaping (Swift.Bool) -> ()) { _skeleton() }
}

extension SwiftUI.TransferableDropDelegate: SwiftUI.DropDelegate {
}
