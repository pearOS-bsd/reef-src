// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _PaddingLayout (Layout).

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

public struct _PaddingLayout {
    public var edges: SwiftUI.Edge.Set
    public var insets: SwiftUI.EdgeInsets?
    /// each element of the content (under "padding") inside the insets (16 on each side by default) of the chosen edges
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] {
        let all = insets ?? SwiftUI.EdgeInsets(_all: 16)
        let used = SwiftUI.EdgeInsets(top: edges.contains(.top) ? all.top : 0, leading: edges.contains(.leading) ? all.leading : 0,
                                      bottom: edges.contains(.bottom) ? all.bottom : 0, trailing: edges.contains(.trailing) ? all.trailing : 0)
        var context = a0
        context.path.append(AnyHashable("padding"))
        // Reef pads the cells of a grid row one by one (GridRowElement); not written yet: the row is padded as a whole
        return a1(context).map { SwiftUI.PaddingElement($0, insets: used) }
    }
    public init(edges a0: SwiftUI.Edge.Set, insets a1: SwiftUI.EdgeInsets?) {
        edges = a0
        insets = a1
    }
}

extension SwiftUI._PaddingLayout: SwiftUI.PrimitiveModifier {
}

extension SwiftUI._PaddingLayout: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI._PaddingLayout: SwiftUI.Animatable {
    public typealias AnimatableData = SwiftUI.EmptyAnimatableData
}
