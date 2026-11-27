// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CoordinateSpaceProtocol (Layout).

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

public protocol CoordinateSpaceProtocol {
    var coordinateSpace: SwiftUI.CoordinateSpace { get }
}

extension SwiftUI.CoordinateSpaceProtocol where Self == SwiftUI.LocalCoordinateSpace {
    public static var local: SwiftUI.LocalCoordinateSpace { _skeleton() }
}

extension SwiftUI.CoordinateSpaceProtocol where Self == SwiftUI.GlobalCoordinateSpace {
    public static var global: SwiftUI.GlobalCoordinateSpace { _skeleton() }
}

extension SwiftUI.CoordinateSpaceProtocol where Self == SwiftUI.NamedCoordinateSpace {
    public static func named<A1>(_ a0: A1) -> SwiftUI.NamedCoordinateSpace where A1: Swift.Hashable { _skeleton() }
    public static var scrollView: SwiftUI.NamedCoordinateSpace { _skeleton() }
    public static func scrollView(axis a0: SwiftUI.Axis) -> SwiftUI.NamedCoordinateSpace { _skeleton() }
}
