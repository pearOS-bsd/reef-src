// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabContent (Navigation).

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

public protocol TabContent {
    associatedtype TabValue: Swift.Hashable
    associatedtype Body: SwiftUI.TabContent
    associatedtype _IdentifiedView
    var body: Self.Body { get }
    var _identifiedView: Self._IdentifiedView { get }
}

extension SwiftUI.TabContent {
    public var _identifiedView: Self { _skeleton() }
    public func accessibilityIdentifier(_ a0: Swift.String, isEnabled a1: Swift.Bool) -> some SwiftUI.TabContent { self }
}
