// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DragSession (Gestures).

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

public struct DragSession {
    public internal(set) var phase: SwiftUI.DragSession.Phase
    var items: [Swift.AnyHashable]
    public func draggedItemIDs<A>(`for` a0: A.Type) -> [A] where A: Swift.Hashable { _skeleton() }
    public enum Phase {
        case ended(SwiftUI.DropOperation)
        case initial
        case active
        case dataTransferCompleted
        public static func == (_ a0: SwiftUI.DragSession.Phase, _ a1: SwiftUI.DragSession.Phase) -> Swift.Bool { _skeleton() }
    }
}

extension SwiftUI.DragSession.Phase: Swift.Equatable {
}
