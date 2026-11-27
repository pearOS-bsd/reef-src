// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShareLink (Controls).

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

public struct ShareLink<A, B, C, D> where A: Swift.RandomAccessCollection, B: CoreTransferable.Transferable, C: CoreTransferable.Transferable, D: SwiftUI.View, A.Element: CoreTransferable.Transferable {
    let items: A
    let label: D
    public init<A1>(item a0: A1, subject a1: SwiftUI.Text?, message a2: SwiftUI.Text?, preview a3: SwiftUI.SharePreview<B, C>, label a4: () -> D) where A == Swift.CollectionOfOne<A1>, A1: CoreTransferable.Transferable { _skeleton() }
}

extension SwiftUI.ShareLink where D == SwiftUI.DefaultShareLinkLabel {
    public init<A1>(item a0: A1, subject a1: SwiftUI.Text?, message a2: SwiftUI.Text?, preview a3: SwiftUI.SharePreview<B, C>) where A == Swift.CollectionOfOne<A1>, A1: CoreTransferable.Transferable { _skeleton() }
}

extension SwiftUI.ShareLink where B == Swift.Never, C == Swift.Never, D == SwiftUI.DefaultShareLinkLabel {
    public init(item a0: Swift.String, subject a1: SwiftUI.Text?, message a2: SwiftUI.Text?) where A == Swift.CollectionOfOne<Swift.String> { _skeleton() }
    public init(item a0: Foundation.URL, subject a1: SwiftUI.Text?, message a2: SwiftUI.Text?) where A == Swift.CollectionOfOne<Foundation.URL> { _skeleton() }
}

extension SwiftUI.ShareLink: SwiftUI.View {
    public typealias Body = Swift.Never
}
