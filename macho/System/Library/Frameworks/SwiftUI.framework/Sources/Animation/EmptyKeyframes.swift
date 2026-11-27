// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EmptyKeyframes (Animation).

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

struct EmptyKeyframes<A> where A: SwiftUI.Animatable {
    init() { _skeleton() }
}

extension SwiftUI.EmptyKeyframes: SwiftUI.KeyframeTrackContent {
    typealias Value = A
}
