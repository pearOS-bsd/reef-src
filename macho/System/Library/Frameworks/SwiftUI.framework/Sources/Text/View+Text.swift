// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Text area.

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
    public func accessibilityInputLabels<A1>(_ a0: [A1]) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func accessibilityLabel(_ a0: SwiftUI.Text, isEnabled a1: Swift.Bool) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func textEditorStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.TextEditorStyle { self }
    public func textSelection<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.TextSelectability { self }
    public func font(_ a0: SwiftUI.Font?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.font, value: a0)) }
    public func textCase(_ a0: SwiftUI.Text.Case?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.textCase, value: a0)) }
    public func kerning(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func tracking(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func fontDesign(_ a0: SwiftUI.Font.Design?) -> some SwiftUI.View { self }
    public func fontWeight(_ a0: SwiftUI.Font.Weight?) -> some SwiftUI.View { self }
    public func lineLimit(_ a0: Swift.ClosedRange<Swift.Int>) -> some SwiftUI.View { self }
    public func lineLimit(_ a0: Swift.PartialRangeFrom<Swift.Int>) -> some SwiftUI.View { self }
    public func lineLimit(_ a0: Swift.PartialRangeThrough<Swift.Int>) -> some SwiftUI.View { self }
    public func fileDialogConfirmationLabel<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { self }
    public func labelsVisibility(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { self }
    public func labelReservedIconWidth(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func textScale(_ a0: SwiftUI.Text.Scale, isEnabled a1: Swift.Bool) -> some SwiftUI.View { self }
    public func accessibilityLabeledPair<A1>(role a0: SwiftUI.AccessibilityLabeledPairRole, id a1: A1, `in` a2: SwiftUI.Namespace.ID) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func fileDialogConfirmationLabel(_ a0: SwiftUI.Text?) -> some SwiftUI.View { self }
    public func labelsHidden() -> some SwiftUI.View { self }
    public func lineLimit(_ a0: Swift.Int?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.lineLimit, value: a0)) }
    public func lineLimit(_ a0: Swift.Int, reservesSpace a1: Swift.Bool) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.lineLimit, value: Optional(a0))) }
    public func truncationMode(_ a0: SwiftUI.Text.TruncationMode) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.truncationMode, value: a0)) }
    public func fontWidth(_ a0: SwiftUI.Font.Width?) -> some SwiftUI.View { self }
    public func accessibilityLabel(_ a0: SwiftUI.Text) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityLabel(_ a0: SwiftUI.LocalizedStringKey) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityLabel<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> where A1: Swift.StringProtocol { _skeleton() }
    public func labelStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.LabelStyle { self }
    public func labeledContentStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.LabeledContentStyle { self }
    public func textFieldStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.TextFieldStyle { self }
}
