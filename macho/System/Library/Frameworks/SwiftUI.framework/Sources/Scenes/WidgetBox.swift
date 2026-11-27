// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WidgetBox (Scenes).

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

struct WidgetBox<A> where A: SwiftUI.Widget {
    let content: A
    init() { _skeleton() }
    init(content a0: A) { _skeleton() }
}

extension SwiftUI.WidgetBox: SwiftUI.Widget {
    typealias Body = Swift.Never
    var body: Self.Body { _skeleton() }
}
