// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollTargetBehaviorContext (Layout).

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

public struct ScrollTargetBehaviorContext {
    public var originalTarget: SwiftUI.ScrollTarget
    public var velocity: CGVector
    public var contentSize: CGSize
    public var containerSize: CGSize
    public var axes: SwiftUI.Axis.Set
    var environment: SwiftUI.EnvironmentValues
    public subscript<A>(dynamicMember a0: Swift.KeyPath<SwiftUI.EnvironmentValues, A>) -> A { _skeleton() }
}
