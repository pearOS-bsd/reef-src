// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnyViewStorage (State).

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

class AnyViewStorage<A>: SwiftUI.AnyViewStorageBase where A: SwiftUI.View {
    let view: A
    init(_ a0: A) {
        view = a0
        super.init()
    }
    /// the view under its type, so that a new type makes new state
    override func buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var context = a0
        context.path.append(AnyHashable(SwiftUI.TypeKey(type: A.self)))
        return SwiftUI.ViewBuild.build(view, context)
    }
    override var anyView: any SwiftUI.View { view }
    deinit {}
}
