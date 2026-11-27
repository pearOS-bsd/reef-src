// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// KeyframeAnimatorBody (Animation).

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

struct KeyframeAnimatorBody<A, B, C> where C: SwiftUI.View {
    let initialValue: B
    let trigger: Swift.AnyHashable?
    let repeating: Swift.Bool
    let content: (SwiftUI.PlaceholderContentView<A>, B) -> C
    let timeline: (B) -> SwiftUI.KeyframeTimeline<B>
    var _start: SwiftUI.State<Foundation.Date?>
    var body: some SwiftUI.View { _skeleton() }
    func frame(_ a0: SwiftUI.KeyframeTimeline<B>, started a1: Foundation.Date?, at a2: Foundation.Date) -> C { _skeleton() }
    init(initialValue a0: B, trigger a1: Swift.AnyHashable?, repeating a2: Swift.Bool, content a3: @escaping (SwiftUI.PlaceholderContentView<A>, B) -> C, timeline a4: @escaping (B) -> SwiftUI.KeyframeTimeline<B>) { _skeleton() }
}

extension SwiftUI.KeyframeAnimatorBody: SwiftUI.View {
}
