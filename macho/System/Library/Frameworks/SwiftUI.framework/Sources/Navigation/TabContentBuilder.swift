// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabContentBuilder (Navigation).

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

public struct TabContentBuilder<A> where A: Swift.Hashable {
    public static func buildIf<A1>(_ a0: A1?) -> SwiftUI._OptionalTabContent<A, A1> where A == A1.TabValue, A1: SwiftUI.TabContent { _skeleton() }
    public static func buildEither<A1, B1>(first a0: A1) -> SwiftUI._ConditionalTabContent<A, A1, B1> where A == A1.TabValue, A1: SwiftUI.TabContent, B1: SwiftUI.TabContent, A1.TabValue == B1.TabValue { _skeleton() }
    public static func buildEither<A1, B1>(second a0: B1) -> SwiftUI._ConditionalTabContent<A, A1, B1> where A == A1.TabValue, A1: SwiftUI.TabContent, B1: SwiftUI.TabContent, A1.TabValue == B1.TabValue { _skeleton() }
    public struct Content<A1> where A1: SwiftUI.TabContent {
        let content: A1
        public var body: Swift.Never { _skeleton() }
        init(content a0: A1) { _skeleton() }
        var tabPages: [SwiftUI.TabPage] { _skeleton() }
    }
}

extension SwiftUI.TabContentBuilder.Content: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TabContentBuilder.Content: SwiftUI.TabPagesProviding {
}
