// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Accessibility area.

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
    public func accessibilityAction(_ a0: SwiftUI.AccessibilityActionKind, _ a1: @escaping () -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityAction<A1>(named a0: A1, _ a1: @escaping () -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityActions<A1>(_ a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func accessibility(addTraits a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibility(hidden a0: Swift.Bool) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibility(label a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHint<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibility(identifier a0: Swift.String) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilitySortPriority(_ a0: Swift.Double) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityRotor<A1, B1>(_ a0: SwiftUI.Text, entries a1: [A1], entryID a2: Swift.KeyPath<A1, B1>, entryLabel a3: Swift.KeyPath<A1, Swift.String>) -> some SwiftUI.View where B1: Swift.Hashable { self }
    public func accessibilityHint(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityElement(children a0: SwiftUI.AccessibilityChildBehavior) -> some SwiftUI.View { self }
    public func accessibilityAdjustableAction(_ a0: @escaping (SwiftUI.AccessibilityAdjustmentDirection) -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityIdentifier(_ a0: Swift.String) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityValue(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityValue(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHint(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityShowsLargeContentViewer() -> some SwiftUI.View { self }
    public func accessibilityRespondsToUserInteraction(_ a0: Swift.Bool) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityHidden(_ a0: Swift.Bool) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityValue<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityShowsLargeContentViewer<A1>(_ a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func accessibilityRepresentation<A1>(representation a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
}
