// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewSpacing (Layout).

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

public struct ViewSpacing {
    weak var element: SwiftUI.Element?
    var isZero: Swift.Bool
    public init() { _skeleton() }
    public static var zero: SwiftUI.ViewSpacing { _skeleton() }
    public func distance(to a0: SwiftUI.ViewSpacing, along a1: SwiftUI.Axis) -> CoreGraphics.CGFloat { _skeleton() }
}
