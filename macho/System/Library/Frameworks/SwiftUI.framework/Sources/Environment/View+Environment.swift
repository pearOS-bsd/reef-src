// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Environment area.

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
    func environmentSet(_ a0: @escaping (inout SwiftUI.EnvironmentValues) -> ()) -> SwiftUI.ModifiedContent<Self, SwiftUI.EnvironmentSetModifier> { _skeleton() }
    public func environment<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A1>, _ a1: A1) -> some SwiftUI.View {
        modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: a0, value: a1))
    }
    public func transformEnvironment<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A1>, transform a1: @escaping (inout A1) -> ()) -> some SwiftUI.View { self }
    public func environmentObject<A1>(_ a0: A1) -> some SwiftUI.View where A1: Combine.ObservableObject { self }
    public func preferredColorScheme(_ a0: SwiftUI.ColorScheme?) -> some SwiftUI.View { self }
    public func preference<A1>(key a0: A1.Type, value a1: A1.Value) -> some SwiftUI.View where A1: SwiftUI.PreferenceKey { self }
    public func onPreferenceChange<A1>(_ a0: A1.Type, perform a1: @escaping (A1.Value) -> ()) -> some SwiftUI.View where A1: SwiftUI.PreferenceKey, A1.Value: Swift.Equatable { self }
    public func controlSize(_ a0: SwiftUI.ControlSize) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.controlSize, value: a0)) }
    public func environment<A1>(_ a0: A1?) -> some SwiftUI.View where A1: AnyObject, A1: Observation.Observable { self }
    public func transformPreference<A1>(_ a0: A1.Type, _ a1: @escaping (inout A1.Value) -> ()) -> some SwiftUI.View where A1: SwiftUI.PreferenceKey { self }
    public func overlayPreferenceValue<A1, B1>(_ a0: A1.Type, alignment a1: SwiftUI.Alignment, _ a2: @escaping (A1.Value) -> B1) -> some SwiftUI.View where A1: SwiftUI.PreferenceKey, B1: SwiftUI.View { self }
    public func anchorPreference<A1, B1>(key a0: B1.Type, value a1: SwiftUI.Anchor<A1>.Source, transform a2: @escaping (SwiftUI.Anchor<A1>) -> B1.Value) -> some SwiftUI.View where B1: SwiftUI.PreferenceKey { self }
    public func transformAnchorPreference<A1, B1>(key a0: B1.Type, value a1: SwiftUI.Anchor<A1>.Source, transform a2: @escaping (inout B1.Value, SwiftUI.Anchor<A1>) -> ()) -> some SwiftUI.View where B1: SwiftUI.PreferenceKey { self }
    public func dynamicTypeSize(_ a0: SwiftUI.DynamicTypeSize) -> some SwiftUI.View { self }
    public func accessibilityAddTraits(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func accessibilityRemoveTraits(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.ModifiedContent<Self, SwiftUI.AccessibilityAttachmentModifier> { _skeleton() }
    public func dynamicTypeSize<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.RangeExpression, A1.Bound == SwiftUI.DynamicTypeSize { self }
}
