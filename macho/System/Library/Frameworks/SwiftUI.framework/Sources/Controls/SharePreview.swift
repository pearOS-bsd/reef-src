// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SharePreview (Controls).

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

public struct SharePreview<A, B> where A: CoreTransferable.Transferable, B: CoreTransferable.Transferable {
    let title: SwiftUI.Text
}

extension SwiftUI.SharePreview where B == Swift.Never {
    public init(_ a0: SwiftUI.Text, image a1: A) { _skeleton() }
    public init<A1>(_ a0: A1, image a1: A) where A1: Swift.StringProtocol { _skeleton() }
}
