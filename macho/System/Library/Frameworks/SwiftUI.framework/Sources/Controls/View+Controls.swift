// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Controls area.

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
    public func focused(_ a0: SwiftUI.FocusState<Swift.Bool>.Binding) -> some SwiftUI.View { self }
    public func focusedValue<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.FocusedValues, A1?>, _ a1: A1) -> some SwiftUI.View { self }
    public func focusedValue<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.FocusedValues, A1?>, _ a1: A1?) -> some SwiftUI.View { self }
    public func focusedSceneValue<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.FocusedValues, A1?>, _ a1: A1) -> some SwiftUI.View { self }
    public func focusedObject<A1>(_ a0: A1) -> some SwiftUI.View where A1: Combine.ObservableObject { self }
    public func focusedSceneObject<A1>(_ a0: A1) -> some SwiftUI.View where A1: Combine.ObservableObject { self }
    public func keyboardShortcut(_ a0: SwiftUI.KeyboardShortcut) -> some SwiftUI.View { self }
    public func compositingGroup() -> some SwiftUI.View { self }
    public func buttonSizing(_ a0: SwiftUI.ButtonSizing) -> some SwiftUI.View { self }
    public func focusScope(_ a0: SwiftUI.Namespace.ID) -> some SwiftUI.View { self }
    public func prefersDefaultFocus(_ a0: Swift.Bool, `in` a1: SwiftUI.Namespace.ID) -> some SwiftUI.View { self }
    public func help(_ a0: SwiftUI.Text) -> some SwiftUI.View { self }
    public func badge(_ a0: Swift.Int) -> some SwiftUI.View { self }
    public func badge(_ a0: SwiftUI.Text?) -> some SwiftUI.View { self }
    public func selectionDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func keyboardShortcut(_ a0: SwiftUI.KeyboardShortcut?) -> some SwiftUI.View { self }
    public func focusable(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func focused<A1>(_ a0: SwiftUI.FocusState<A1>.Binding, equals a1: A1) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func help(_ a0: SwiftUI.LocalizedStringKey) -> some SwiftUI.View { self }
    public func help<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { self }
    public func keyboardShortcut(_ a0: SwiftUI.KeyEquivalent, modifiers a1: SwiftUI.EventModifiers) -> some SwiftUI.View { self }
    public func listSectionSeparator(_ a0: SwiftUI.Visibility, edges a1: SwiftUI.VerticalEdge.Set) -> some SwiftUI.View { self }
    public func onSubmit(of a0: SwiftUI.SubmitTriggers, _ a1: @escaping () -> ()) -> some SwiftUI.View { self }
    public func scrollDismissesKeyboard(_ a0: SwiftUI.ScrollDismissesKeyboardMode) -> some SwiftUI.View { self }
    public func accessibilityFocused(_ a0: SwiftUI.AccessibilityFocusState<Swift.Bool>.Binding) -> some SwiftUI.View { self }
    public func accessibilityFocused<A1>(_ a0: SwiftUI.AccessibilityFocusState<A1>.Binding, equals a1: A1) -> some SwiftUI.View where A1: Swift.Hashable { self }
}
