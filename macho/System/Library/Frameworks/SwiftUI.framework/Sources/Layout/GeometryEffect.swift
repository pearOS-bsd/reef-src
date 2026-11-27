// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryEffect (Layout).

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

public protocol GeometryEffect: SwiftUI.Animatable, SwiftUI.ViewModifier where Self.Body == Swift.Never {
    func effectValue(size a0: CGSize) -> SwiftUI.ProjectionTransform
}
