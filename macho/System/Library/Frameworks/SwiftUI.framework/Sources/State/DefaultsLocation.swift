// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DefaultsLocation (State).

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

class DefaultsLocation<A>: SwiftUI.AnyLocation<A> {
    let read: () -> A = _skeletonValue()
    let write: (A) -> () = _skeletonValue()
    let store: UserDefaults = _skeletonValue()
    let key: Swift.String = _skeletonValue()
    init(get a0: @escaping () -> A, set a1: @escaping (A) -> (), store a2: UserDefaults, key a3: Swift.String) { _skeleton() }
    override func get() -> A { _skeleton() }
    override func set(_ a0: A, transaction a1: SwiftUI.Transaction) { _skeleton() }
    deinit {}
}
