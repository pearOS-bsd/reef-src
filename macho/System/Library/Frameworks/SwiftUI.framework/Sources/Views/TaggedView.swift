// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TaggedView (Views).

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

protocol TaggedView {
    var tagValues: [Swift.AnyHashable] { get }
    var taggedContent: Any { get }
}

extension SwiftUI.TaggedView {
    func tagValue(`for` a0: Any.Type) -> Swift.AnyHashable? { _skeleton() }
}
