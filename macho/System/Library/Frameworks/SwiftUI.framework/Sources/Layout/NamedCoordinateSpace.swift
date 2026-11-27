// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NamedCoordinateSpace (Layout).

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

public struct NamedCoordinateSpace {
    let name: Swift.AnyHashable
    public var coordinateSpace: SwiftUI.CoordinateSpace { _skeleton() }
    public static func == (_ a0: SwiftUI.NamedCoordinateSpace, _ a1: SwiftUI.NamedCoordinateSpace) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.NamedCoordinateSpace: SwiftUI.CoordinateSpaceProtocol {
}

extension SwiftUI.NamedCoordinateSpace: Swift.Equatable {
}
