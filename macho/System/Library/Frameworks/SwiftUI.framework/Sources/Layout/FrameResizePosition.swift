// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FrameResizePosition (Layout).

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

@frozen public enum FrameResizePosition {
    case top
    case leading
    case bottom
    case trailing
    case topLeading
    case topTrailing
    case bottomLeading
    case bottomTrailing
    public init?(rawValue a0: Swift.Int8) { _skeleton() }
    public static var allCases: [SwiftUI.FrameResizePosition] { _skeleton() }
    public var rawValue: Swift.Int8 { _skeleton() }
}

extension SwiftUI.FrameResizePosition: Swift.Equatable {
}

extension SwiftUI.FrameResizePosition: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int8
}

extension SwiftUI.FrameResizePosition: Swift.CaseIterable {
    public typealias AllCases = [SwiftUI.FrameResizePosition]
}

extension SwiftUI.FrameResizePosition: Swift.Hashable {
}
