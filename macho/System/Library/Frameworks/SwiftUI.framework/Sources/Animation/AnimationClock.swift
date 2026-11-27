// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationClock (Animation).

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

class AnimationClock {
    var timer: Timer? = _skeletonValue()
    var clients: [Swift.ObjectIdentifier: () -> Swift.Bool] = _skeletonValue()
    let frameInterval: Swift.Double = _skeletonValue()
    final func add(_ a0: AnyObject, _ a1: @escaping () -> Swift.Bool) { _skeleton() }
    init() { _skeleton() }
    static var shared: SwiftUI.AnimationClock { _skeleton() }
    deinit {}
}
