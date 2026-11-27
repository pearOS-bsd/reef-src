// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropDelegate (Gestures).

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

public protocol DropDelegate {
    func validateDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool
    func dropEntered(info a0: SwiftUI.DropInfo)
    func dropUpdated(info a0: SwiftUI.DropInfo) -> SwiftUI.DropProposal?
    func dropExited(info a0: SwiftUI.DropInfo)
    func performDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool
}

extension SwiftUI.DropDelegate {
    public func validateDrop(info a0: SwiftUI.DropInfo) -> Swift.Bool { _skeleton() }
    public func dropEntered(info a0: SwiftUI.DropInfo) { _skeleton() }
    public func dropUpdated(info a0: SwiftUI.DropInfo) -> SwiftUI.DropProposal? { _skeleton() }
    public func dropExited(info a0: SwiftUI.DropInfo) { _skeleton() }
}
