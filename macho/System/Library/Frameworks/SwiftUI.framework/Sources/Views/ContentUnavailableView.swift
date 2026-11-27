// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ContentUnavailableView (Views).

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

public struct ContentUnavailableView<A, B, C> where A: SwiftUI.View, B: SwiftUI.View, C: SwiftUI.View {
    let label: A
    let description: B
    let actions: C
    public init(label a0: () -> A, description a1: () -> B, actions a2: () -> C) { _skeleton() }
    public var body: some SwiftUI.View { _skeleton() }
}

extension SwiftUI.ContentUnavailableView where A == SwiftUI.Label<SwiftUI.Text, SwiftUI.Image>, B == SwiftUI.Text?, C == SwiftUI.EmptyView {
    public init(_ a0: SwiftUI.LocalizedStringKey, systemImage a1: Swift.String, description a2: SwiftUI.Text?) { _skeleton() }
    public init<A1>(_ a0: A1, systemImage a1: Swift.String, description a2: SwiftUI.Text?) where A1: Swift.StringProtocol { _skeleton() }
}

extension SwiftUI.ContentUnavailableView where A == SwiftUI.SearchUnavailableContent.Label, B == SwiftUI.SearchUnavailableContent.Description, C == SwiftUI.SearchUnavailableContent.Actions {
    public static var search: SwiftUI.ContentUnavailableView<SwiftUI.SearchUnavailableContent.Label, SwiftUI.SearchUnavailableContent.Description, SwiftUI.SearchUnavailableContent.Actions> { _skeleton() }
    public static func search(text a0: Swift.String) -> SwiftUI.ContentUnavailableView<SwiftUI.SearchUnavailableContent.Label, SwiftUI.SearchUnavailableContent.Description, SwiftUI.SearchUnavailableContent.Actions> { _skeleton() }
}

extension SwiftUI.ContentUnavailableView: SwiftUI.View {
}
