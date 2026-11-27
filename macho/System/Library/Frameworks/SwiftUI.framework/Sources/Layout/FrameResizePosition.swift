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
    public init?(rawValue a0: Swift.Int8) {
        switch a0 {
        case 0: self = .top
        case 1: self = .leading
        case 2: self = .bottom
        case 3: self = .trailing
        case 4: self = .topLeading
        case 5: self = .topTrailing
        case 6: self = .bottomLeading
        case 7: self = .bottomTrailing
        default: return nil
        }
    }
    public static var allCases: [SwiftUI.FrameResizePosition] { [.top, .leading, .bottom, .trailing, .topLeading, .topTrailing, .bottomLeading, .bottomTrailing] }
    public var rawValue: Swift.Int8 {
        switch self {
        case .top: return 0
        case .leading: return 1
        case .bottom: return 2
        case .trailing: return 3
        case .topLeading: return 4
        case .topTrailing: return 5
        case .bottomLeading: return 6
        case .bottomTrailing: return 7
        }
    }
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
