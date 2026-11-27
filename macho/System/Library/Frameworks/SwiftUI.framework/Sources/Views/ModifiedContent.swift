// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ModifiedContent (Views).

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

public struct ModifiedContent<A, B> {
    public var content: A
    public var modifier: B
    public init(content a0: A, modifier a1: B) { _skeleton() }
    var writesTag: Swift.Bool { _skeleton() }
    var contentPart: Any { _skeleton() }
}

extension SwiftUI.ModifiedContent where B == SwiftUI.AccessibilityAttachmentModifier {
    public func accessibility(addTraits a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityAction(_ a0: SwiftUI.AccessibilityActionKind, _ a1: @escaping () -> ()) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityAction<A1>(named a0: A1, _ a1: @escaping () -> ()) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibility(hidden a0: Swift.Bool) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHint<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityIdentifier(_ a0: Swift.String) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityInputLabels<A1>(_ a0: [A1]) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilitySortPriority(_ a0: Swift.Double) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHint(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityAdjustableAction(_ a0: @escaping (SwiftUI.AccessibilityAdjustmentDirection) -> ()) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHint(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityLabel<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityValue<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityHeading(_ a0: SwiftUI.AccessibilityHeadingLevel) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHidden(_ a0: Swift.Bool) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityRespondsToUserInteraction(_ a0: Swift.Bool) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityAddTraits(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityRemoveTraits(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityValue(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityValue(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityLabel(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<A, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
}

extension SwiftUI.ModifiedContent where A: SwiftUI.View, B: SwiftUI.ViewModifier {
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
    func menuEntries(_ a0: SwiftUI.EnvironmentValues) -> [SwiftUI.MenuEntry] { _skeleton() }
    var tagValues: [Swift.AnyHashable] { _skeleton() }
    var taggedContent: Any { _skeleton() }
    var tabItemLabel: SwiftUI.AnyView? { _skeleton() }
}

extension SwiftUI.ModifiedContent where A: SwiftUI.ViewModifier, B: SwiftUI.ViewModifier {
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.ModifiedContent where A: SwiftUI.Scene, B: SwiftUI._SceneModifier {
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.ModifiedContent where A: SwiftUI.DynamicViewContent, B: SwiftUI.ViewModifier {
    public var data: A.Data { _skeleton() }
}

extension SwiftUI.ModifiedContent where A: SwiftUI.TableRowContent, B: SwiftUI._TableRowContentModifier {
    public var tableRowBody: Swift.Never { _skeleton() }
    func tableRows() -> [SwiftUI.TableRowEntry] { _skeleton() }
}

extension SwiftUI.ModifiedContent: SwiftUI.View where A: SwiftUI.View, B: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}

extension SwiftUI.ModifiedContent: SwiftUI.ViewModifier where A: SwiftUI.ViewModifier, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.PrimitiveView where A: SwiftUI.View, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.PrimitiveModifier where A: SwiftUI.ViewModifier, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.MenuContributor where A: SwiftUI.View, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.TaggedView where A: SwiftUI.View, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.TagWritingContent {
}

extension SwiftUI.ModifiedContent: SwiftUI.ModifiedContentParts {
}

extension SwiftUI.ModifiedContent: SwiftUI.TabItemReading where A: SwiftUI.View, B: SwiftUI.ViewModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.PrimitiveScene where A: SwiftUI.Scene, B: SwiftUI._SceneModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.DynamicViewContent where A: SwiftUI.DynamicViewContent, B: SwiftUI.ViewModifier {
    public typealias Data = A.Data
}

extension SwiftUI.ModifiedContent: SwiftUI.TableRowContent where A: SwiftUI.TableRowContent, B: SwiftUI._TableRowContentModifier {
    public typealias TableRowValue = A.TableRowValue
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.ModifiedContent: SwiftUI.TableRowsSource where A: SwiftUI.TableRowContent, B: SwiftUI._TableRowContentModifier {
}

extension SwiftUI.ModifiedContent: SwiftUI.Scene where A: SwiftUI.Scene, B: SwiftUI._SceneModifier {
}
