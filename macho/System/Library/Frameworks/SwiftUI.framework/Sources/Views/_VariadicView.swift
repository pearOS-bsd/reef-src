// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _VariadicView (Views).

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

public enum _VariadicView {
    public struct Tree<A, B> where A: SwiftUI._VariadicView_Root {
        public var root: A
        public var content: B
        public init(_ a0: A, content a1: () -> B) {
            root = a0
            content = a1()
        }
        public init(root a0: A, content a1: B) {
            root = a0
            content = a1
        }
    }
}

extension SwiftUI._VariadicView.Tree where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
    /// the content built under "tree" with the stack axis of the root, then one element laid out by the root: a stack,
    /// a z-stack, a grid or what an AnyLayout root makes
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] {
        var context = a0
        context.path.append(AnyHashable("tree"))
        if root is SwiftUI._HStackLayout || root is SwiftUI.HStackLayout {
            context.stackAxis = .horizontal
        } else if root is SwiftUI._VStackLayout || root is SwiftUI.VStackLayout {
            context.stackAxis = .vertical
        } else if root is SwiftUI._ZStackLayout {
            context.stackAxis = nil
        }
        let children = SwiftUI.ViewBuild.build(content, context)
        if let layout = root as? SwiftUI._HStackLayout {
            return [SwiftUI.StackElement(axis: .horizontal, alignment: layout.alignment.key, spacing: layout.spacing, items: children)]
        }
        if let layout = root as? SwiftUI._VStackLayout {
            return [SwiftUI.StackElement(axis: .vertical, alignment: layout.alignment.key, spacing: layout.spacing, items: children)]
        }
        if let layout = root as? SwiftUI._ZStackLayout {
            return [SwiftUI.ZStackElement(alignment: layout.alignment, items: children)]
        }
        if let layout = root as? SwiftUI.GridLayout {
            return [SwiftUI.GridElement(layout: layout, rows: children)]
        }
        guard let layout = root as? any SwiftUI.AnyLayoutRoot else {
            SwiftUI.ViewBuild.reportUnhandled(A.self, "layout root")
            return children
        }
        return [layout.element(children)]
    }
}

extension SwiftUI._VariadicView.Tree: SwiftUI.PrimitiveView where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
}

extension SwiftUI._VariadicView.Tree: SwiftUI.View where A: SwiftUI._VariadicView_ViewRoot, B: SwiftUI.View {
    public typealias Body = Swift.Never
}
