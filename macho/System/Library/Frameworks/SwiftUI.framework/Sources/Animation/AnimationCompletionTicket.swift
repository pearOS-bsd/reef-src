// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationCompletionTicket (Animation).

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

class AnimationCompletionTicket {
    let criteria: SwiftUI.AnimationCompletionCriteria = _skeletonValue()
    var completion: (() -> ())? = _skeletonValue()
    var running: Swift.Int = _skeletonValue()
    var claimed: Swift.Bool = _skeletonValue()
    init(criteria a0: SwiftUI.AnimationCompletionCriteria, completion a1: @escaping () -> ()) { _skeleton() }
    deinit {}
}
