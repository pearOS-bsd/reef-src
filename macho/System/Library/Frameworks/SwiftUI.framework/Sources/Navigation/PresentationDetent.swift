// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationDetent (Navigation).

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

public struct PresentationDetent {
    let id: Swift.Int
    let height: CoreGraphics.CGFloat?
    public static var medium: SwiftUI.PresentationDetent { _skeleton() }
    public static var large: SwiftUI.PresentationDetent { _skeleton() }
    public static func height(_ a0: CoreGraphics.CGFloat) -> SwiftUI.PresentationDetent { _skeleton() }
    public static func fraction(_ a0: CoreGraphics.CGFloat) -> SwiftUI.PresentationDetent { _skeleton() }
    public static func == (_ a0: SwiftUI.PresentationDetent, _ a1: SwiftUI.PresentationDetent) -> Swift.Bool { _skeleton() }
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
}

extension SwiftUI.PresentationDetent: Swift.Hashable {
}

extension SwiftUI.PresentationDetent: Swift.Equatable {
}
