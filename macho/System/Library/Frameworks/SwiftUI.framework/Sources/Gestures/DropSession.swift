// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropSession (Gestures).

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

public struct DropSession {
    public let location: CGPoint
    public let localSession: SwiftUI.DropSession.LocalSession?
    public struct LocalSession {
        let items: [Swift.AnyHashable]
        public func draggedItemIDs<A>(`for` a0: A.Type) -> [A] where A: Swift.Hashable { _skeleton() }
    }
}
