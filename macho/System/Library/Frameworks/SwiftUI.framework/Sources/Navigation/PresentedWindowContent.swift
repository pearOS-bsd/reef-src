// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentedWindowContent (Navigation).

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

public struct PresentedWindowContent<A, B> {
    let presented: SwiftUI.PresentedValue
    let content: (SwiftUI.Binding<A?>) -> B
    let defaultValue: (() -> A)?
    init(presented a0: SwiftUI.PresentedValue, content a1: @escaping (SwiftUI.Binding<A?>) -> B, defaultValue a2: (() -> A)?) { _skeleton() }
}

extension SwiftUI.PresentedWindowContent where A: Swift.Decodable, A: Swift.Encodable, A: Swift.Hashable, B: SwiftUI.View {
    public var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.PresentedWindowContent: SwiftUI.View where A: Swift.Decodable, A: Swift.Encodable, A: Swift.Hashable, B: SwiftUI.View {
}
