// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeSegmentSource (Animation).

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

protocol KeyframeSegmentSource {
    func collectSegments<A1>(into a0: inout [SwiftUI.KeyframeSegment<A1>]) where A1: SwiftUI.Animatable
}
