// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicViewContent (Lists).

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

public protocol DynamicViewContent: SwiftUI.View {
    associatedtype Data: Swift.Collection
    var data: Self.Data { get }
}

extension SwiftUI.DynamicViewContent {
    public func onMove(perform a0: ((Foundation.IndexSet, Swift.Int) -> ())?) -> some SwiftUI.DynamicViewContent { self }
}
