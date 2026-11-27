// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Misc area.

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
    public func onReceive<A1>(_ a0: A1, perform a1: @escaping (A1.Output) -> ()) -> some SwiftUI.View where A1: Combine.Publisher, A1.Failure == Swift.Never { self }
    public func onKeyPress(phases a0: SwiftUI.KeyPress.Phases, action a1: @escaping (SwiftUI.KeyPress) -> SwiftUI.KeyPress.Result) -> some SwiftUI.View { self }
    public func onChange<A1>(of a0: A1, initial a1: Swift.Bool, _ a2: @escaping () -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func id<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func onAppear(perform a0: (() -> ())?) -> some SwiftUI.View { self }
    public func onDisappear(perform a0: (() -> ())?) -> some SwiftUI.View { self }
    /// isEnabled stays true only where it was and a0 is false
    public func disabled(_ a0: Swift.Bool) -> some SwiftUI.View {
        modifier(SwiftUI._EnvironmentKeyTransformModifier(keyPath: \.isEnabled, transform: { $0 = $0 && !a0 }))
    }
    public func allowsHitTesting(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func tag<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func _trait<A1>(_ a0: A1.Type, _ a1: A1.Value) -> some SwiftUI.View where A1: SwiftUI._ViewTraitKey { self }
    public func _untagged() -> some SwiftUI.View { self }
    public func zIndex(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func minimumScaleFactor(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.minimumScaleFactor, value: a0)) }
    public func underline(_ a0: Swift.Bool, pattern a1: SwiftUI.Text.LineStyle.Pattern, color a2: SwiftUI.Color?) -> some SwiftUI.View { self }
    public func border<A1>(_ a0: A1, width a1: CoreGraphics.CGFloat) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
    public func cornerRadius(_ a0: CoreGraphics.CGFloat, antialiased a1: Swift.Bool) -> some SwiftUI.View { self }
    public func grayscale(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func luminanceToAlpha() -> some SwiftUI.View { self }
    public func task(priority a0: Swift.TaskPriority, _ a1: @escaping @Sendable () async -> ()) -> some SwiftUI.View { self }
    public func hidden() -> some SwiftUI.View { self }
    public func onChange<A1>(of a0: A1, perform a1: @escaping (A1) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func onChange<A1>(of a0: A1, initial a1: Swift.Bool, _ a2: @escaping (A1, A1) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func italic(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func strikethrough(_ a0: Swift.Bool, pattern a1: SwiftUI.Text.LineStyle.Pattern, color a2: SwiftUI.Color?) -> some SwiftUI.View { self }
    public func allowsTightening(_ a0: Swift.Bool) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.allowsTightening, value: a0)) }
    public func monospacedDigit() -> some SwiftUI.View { self }
    public func monospaced(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func task(name a0: Swift.String, executorPreference a1: (any Swift.TaskExecutor)?, priority a2: Swift.TaskPriority, _ a3: __owned @escaping @isolated(any) () async -> ()) -> some SwiftUI.View { self }
    public func task<A1>(id a0: A1, name a1: Swift.String, executorPreference a2: (any Swift.TaskExecutor)?, priority a3: Swift.TaskPriority, _ a4: __owned @escaping @isolated(any) () async -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func bold(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func onKeyPress(_ a0: SwiftUI.KeyEquivalent, phases a1: SwiftUI.KeyPress.Phases, action a2: @escaping (SwiftUI.KeyPress) -> SwiftUI.KeyPress.Result) -> some SwiftUI.View { self }
    public func scaledToFit() -> some SwiftUI.View { self }
    public func onKeyPress(_ a0: SwiftUI.KeyEquivalent, action a1: @escaping () -> SwiftUI.KeyPress.Result) -> some SwiftUI.View { self }
    public func onKeyPress(keys a0: Swift.Set<SwiftUI.KeyEquivalent>, phases a1: SwiftUI.KeyPress.Phases, action a2: @escaping (SwiftUI.KeyPress) -> SwiftUI.KeyPress.Result) -> some SwiftUI.View { self }
    public func headerProminence(_ a0: SwiftUI.Prominence) -> some SwiftUI.View { self }
    public func task<A1>(id a0: A1, priority a1: Swift.TaskPriority, _ a2: @escaping @Sendable () async -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func sensoryFeedback<A1>(_ a0: SwiftUI.SensoryFeedback, trigger a1: A1) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func privacySensitive(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func writingToolsBehavior(_ a0: SwiftUI.WritingToolsBehavior) -> some SwiftUI.View { self }
    public func disableAutocorrection(_ a0: Swift.Bool?) -> some SwiftUI.View { self }
    public func redacted(reason a0: SwiftUI.RedactionReasons) -> some SwiftUI.View { self }
    public func unredacted() -> some SwiftUI.View { self }
    public func copyable<A1>(_ a0: @escaping @autoclosure () -> [A1]) -> some SwiftUI.View where A1: CoreTransferable.Transferable { self }
}
