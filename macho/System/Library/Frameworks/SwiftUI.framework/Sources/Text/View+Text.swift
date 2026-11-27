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
    public func kerning(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.TextKerningKey.self] = a0 }) }
    public func tracking(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.TextTrackingKey.self] = a0 }) }
    public func fontDesign(_ a0: SwiftUI.Font.Design?) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.TextDesignKey.self] = a0 }) }
    public func fontWeight(_ a0: SwiftUI.Font.Weight?) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.TextWeightKey.self] = a0 }) }
    public func lineLimit(_ a0: Swift.ClosedRange<Swift.Int>) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LineLimitKey.self] = a0.upperBound; $0[SwiftUI.LineLimitMinimumKey.self] = a0.lowerBound }) }
    public func lineLimit(_ a0: Swift.PartialRangeFrom<Swift.Int>) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LineLimitKey.self] = nil; $0[SwiftUI.LineLimitMinimumKey.self] = a0.lowerBound }) }
    public func lineLimit(_ a0: Swift.PartialRangeThrough<Swift.Int>) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LineLimitKey.self] = a0.upperBound; $0[SwiftUI.LineLimitMinimumKey.self] = nil }) }
    public func fileDialogConfirmationLabel<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.FileDialogConfirmationLabelKey.self] = Optional(Swift.String(a0)) }) }
    public func labelsVisibility(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LabelsVisibilityKey.self] = a0; if a0 != .automatic { $0[SwiftUI.LabelsHiddenKey.self] = a0 != .visible } }) }
    public func labelReservedIconWidth(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LabelReservedIconWidthKey.self] = Optional(a0) }) }
    public func textScale(_ a0: SwiftUI.Text.Scale, isEnabled a1: Swift.Bool) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { if a1 { $0[SwiftUI.TextScaleKey.self] = Optional(a0) } }) }
    public func accessibilityLabeledPair<A1>(role a0: SwiftUI.AccessibilityLabeledPairRole, id a1: A1, `in` a2: SwiftUI.Namespace.ID) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func fileDialogConfirmationLabel(_ a0: SwiftUI.Text?) -> some SwiftUI.View {
        // the text's plain string: verbatim as is, a storage's plain string
        let string: Swift.String?
        switch a0?.storage {
        case .verbatim(let s)?: string = s
        case .anyTextStorage(let storage)?: string = storage.plainString
        case nil: string = nil
        }
        return modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.FileDialogConfirmationLabelKey.self] = string })
    }
    public func labelsHidden() -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LabelsHiddenKey.self] = true }) }
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
