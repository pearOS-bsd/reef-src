// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Group (Controls).

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

public struct Group<A> {
    public var content: A
    public init<A1, B1>(subviews a0: A1, transform a1: @escaping (SwiftUI.SubviewsCollection) -> B1) where A == SwiftUI.GroupElementsOfContent<A1, B1>, A1: SwiftUI.View, B1: SwiftUI.View { _skeleton() }
    public init(_content a0: A) { content = a0 }
    var contentPart: Any { content }
}

extension SwiftUI.Group where A: SwiftUI.View {
    /// the content under "group"
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var context = a0
        context.path.append(AnyHashable("group"))
        return SwiftUI.ViewBuild.build(content, context)
    }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    public init(@SwiftUI.ViewBuilder content a0: () -> A) { content = a0() }
}

extension SwiftUI.Group where A: SwiftUI.Scene {
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    public init(content a0: () -> A) { _skeleton() }
}

extension SwiftUI.Group where A: SwiftUI.TableRowContent {
    public var tableRowBody: Swift.Never { _skeleton() }
    public init<A1>(content a0: () -> A) where A1 == A.TableRowValue { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

extension SwiftUI.Group: SwiftUI.PrimitiveView where A: SwiftUI.View {
}

extension SwiftUI.Group: SwiftUI.LazyRowSource where A: SwiftUI.View {
}

extension SwiftUI.Group: SwiftUI.MenuContributor where A: SwiftUI.View {
}

extension SwiftUI.Group: SwiftUI.GroupParts {
}

extension SwiftUI.Group: SwiftUI.PrimitiveScene where A: SwiftUI.Scene {
}

extension SwiftUI.Group: SwiftUI.Scene where A: SwiftUI.Scene {
    public typealias Body = Swift.Never
}

extension SwiftUI.Group: SwiftUI.TableRowContent where A: SwiftUI.TableRowContent {
    public typealias TableRowValue = A.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.Group: SwiftUI.TableRowsSource where A: SwiftUI.TableRowContent {
}

extension SwiftUI.Group: SwiftUI.View where A: SwiftUI.View {
}
