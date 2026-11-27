// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ConditionalContent (Views).

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

public struct _ConditionalContent<A, B> {
    public let storage: SwiftUI._ConditionalContent<A, B>.Storage
    public init(storage a0: SwiftUI._ConditionalContent<A, B>.Storage) { storage = a0 }
    var activePart: Any {
        switch storage {
        case let .trueContent(content): return content
        case let .falseContent(content): return content
        }
    }
    @frozen public enum Storage {
        case trueContent(A)
        case falseContent(B)
    }
}

extension SwiftUI._ConditionalContent where A: SwiftUI.View, B: SwiftUI.View {
    /// the active branch, under "true" or "false"
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var context = a0
        switch storage {
        case let .trueContent(content):
            context.path.append(AnyHashable("true"))
            return SwiftUI.ViewBuild.build(content, context)
        case let .falseContent(content):
            context.path.append(AnyHashable("false"))
            return SwiftUI.ViewBuild.build(content, context)
        }
    }
    func _lazyRows(_ a0: SwiftUI.BuildContext) -> [SwiftUI.LazyRow] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    public init(storage a0: SwiftUI._ConditionalContent<A, B>.Storage) { storage = a0 }
}

extension SwiftUI._ConditionalContent where A: SwiftUI.ToolbarContent, B: SwiftUI.ToolbarContent {
    func collectActions(into a0: inout [SwiftUI.ToolbarActions.Item]) { _skeleton() }
}

extension SwiftUI._ConditionalContent where A: SwiftUI.TabContent, B: SwiftUI.TabContent, A.TabValue == B.TabValue {
    public var body: Swift.Never { _skeleton() }
    public init(storage a0: SwiftUI._ConditionalContent<A, B>.Storage) { storage = a0 }
    var tabPages: [SwiftUI.TabPage] { _skeleton() }
}

extension SwiftUI._ConditionalContent where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, A.TableRowValue == B.TableRowValue {
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

extension SwiftUI._ConditionalContent: SwiftUI.View where A: SwiftUI.View, B: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI._ConditionalContent: SwiftUI.PrimitiveView where A: SwiftUI.View, B: SwiftUI.View {
}

extension SwiftUI._ConditionalContent: SwiftUI.LazyRowSource where A: SwiftUI.View, B: SwiftUI.View {
}

extension SwiftUI._ConditionalContent: SwiftUI.MenuContributor where A: SwiftUI.View, B: SwiftUI.View {
}

extension SwiftUI._ConditionalContent: SwiftUI.ConditionalViewParts {
}

extension SwiftUI._ConditionalContent: SwiftUI.ToolbarContent where A: SwiftUI.ToolbarContent, B: SwiftUI.ToolbarContent {
}

extension SwiftUI._ConditionalContent: SwiftUI.ToolbarActionSource where A: SwiftUI.ToolbarContent, B: SwiftUI.ToolbarContent {
}

extension SwiftUI._ConditionalContent: SwiftUI.TabContent where A: SwiftUI.TabContent, B: SwiftUI.TabContent, A.TabValue == B.TabValue {
    public typealias TabValue = A.TabValue
    public typealias _IdentifiedView = SwiftUI._ConditionalContent<A, B>
}

extension SwiftUI._ConditionalContent: SwiftUI.TabPagesProviding where A: SwiftUI.TabContent, B: SwiftUI.TabContent, A.TabValue == B.TabValue {
}

extension SwiftUI._ConditionalContent: SwiftUI.TableRowContent where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, A.TableRowValue == B.TableRowValue {
    public typealias TableRowValue = A.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI._ConditionalContent: SwiftUI.TableRowsSource where A: SwiftUI.TableRowContent, B: SwiftUI.TableRowContent, A.TableRowValue == B.TableRowValue {
}
