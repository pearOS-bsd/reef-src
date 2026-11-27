// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyTransitionBox (Animation).

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

public class AnyTransitionBox {
    let kind: SwiftUI.AnyTransitionBox.Kind = _skeletonValue()
    init(kind a0: SwiftUI.AnyTransitionBox.Kind) { _skeleton() }
    deinit {}
    enum Kind {
        case move(SwiftUI.Edge)
        case identity
        case opacity
        case scale
        case slide
    }
}
