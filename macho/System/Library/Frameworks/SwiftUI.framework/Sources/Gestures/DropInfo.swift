// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DropInfo (Gestures).

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

public struct DropInfo {
    public let location: CGPoint
    let items: [SwiftUI.DropItem]
    let acceptedTypes: [UniformTypeIdentifiers.UTType]
    public func hasItemsConforming(to a0: [UniformTypeIdentifiers.UTType]) -> Swift.Bool { _skeleton() }
    public func itemProviders(`for` a0: [UniformTypeIdentifiers.UTType]) -> [NSItemProvider] { _skeleton() }
}
