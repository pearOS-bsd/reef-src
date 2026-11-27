// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Lists area.

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

extension SwiftUI.View {
    public func accessibilityScrollAction(_ a0: @escaping (SwiftUI.Edge) -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs { _skeleton() }
    public static func _viewListCount(inputs a0: SwiftUI._ViewListCountInputs) -> Swift.Int? { _skeleton() }
    public func scrollDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func moveDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func listRowSeparator(_ a0: SwiftUI.Visibility, edges a1: SwiftUI.VerticalEdge.Set) -> some SwiftUI.View { self }
    public func refreshable(action a0: @escaping @Sendable () async -> ()) -> some SwiftUI.View { self }
    public func scrollBounceBehavior(_ a0: SwiftUI.ScrollBounceBehavior, axes a1: SwiftUI.Axis.Set) -> some SwiftUI.View { self }
    public func scrollIndicators(_ a0: SwiftUI.ScrollIndicatorVisibility, axes a1: SwiftUI.Axis.Set) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { if a1.contains(.vertical) { $0[SwiftUI.VerticalScrollIndicatorsKey.self] = a0 }; if a1.contains(.horizontal) { $0[SwiftUI.HorizontalScrollIndicatorsKey.self] = a0 } }) }
    public func onScrollVisibilityChange(threshold a0: Swift.Double, _ a1: @escaping (Swift.Bool) -> ()) -> some SwiftUI.View { self }
    public func onScrollPhaseChange(_ a0: @escaping (SwiftUI.ScrollPhase, SwiftUI.ScrollPhase, SwiftUI.ScrollPhaseChangeContext) -> ()) -> some SwiftUI.View { self }
    public func onScrollPhaseChange(_ a0: @escaping (SwiftUI.ScrollPhase, SwiftUI.ScrollPhase) -> ()) -> some SwiftUI.View { self }
}
