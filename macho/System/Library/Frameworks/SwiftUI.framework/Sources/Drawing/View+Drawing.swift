// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Drawing area.

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
    public func buttonStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ButtonStyle { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ButtonStyleBoxKey.self] = SwiftUI.CustomButtonStyleBox(a0) }) }
    public func clipShape<A1>(_ a0: A1, style a1: SwiftUI.FillStyle) -> some SwiftUI.View where A1: SwiftUI.Shape { self }
    public func scaleEffect(_ a0: CoreGraphics.CGFloat, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.View { self }
    public func foregroundColor(_ a0: SwiftUI.Color?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.foregroundColor, value: a0)) }
    public func glassEffectID<A1>(_ a0: A1?, `in` a1: SwiftUI.Namespace.ID) -> some SwiftUI.View where A1: Swift.Hashable, A1: Swift.Sendable { self }
    public func colorMultiply(_ a0: SwiftUI.Color) -> some SwiftUI.View { self }
    public func tint(_ a0: SwiftUI.Color?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.tintColor, value: a0)) }
    public func tint<A1>(_ a0: A1?) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
    public func shadow(color a0: SwiftUI.Color, radius a1: CoreGraphics.CGFloat, x a2: CoreGraphics.CGFloat, y a3: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func clipped(antialiased a0: Swift.Bool) -> some SwiftUI.View { self }
    public func blendMode(_ a0: SwiftUI.BlendMode) -> some SwiftUI.View { self }
    public func brightness(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func contrast(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func saturation(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func colorInvert() -> some SwiftUI.View { self }
    public func drawingGroup(opaque a0: Swift.Bool, colorMode a1: SwiftUI.ColorRenderingMode) -> some SwiftUI.View { self }
    public func opacity(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func blur(radius a0: CoreGraphics.CGFloat, opaque a1: Swift.Bool) -> some SwiftUI.View { self }
    public func contentShape<A1>(_ a0: A1, eoFill a1: Swift.Bool) -> some SwiftUI.View where A1: SwiftUI.Shape { self }
    public func buttonStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.PrimitiveButtonStyle { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ButtonStyleBoxKey.self] = SwiftUI.PrimitiveButtonStyleBox(a0) }) }
    public func toggleStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ToggleStyle { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ToggleStyleBoxKey.self] = a0 }) }
    public func progressViewStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ProgressViewStyle { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ProgressStyleKey.self] = a0 }) }
    public func mask<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func scaleEffect(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat, anchor a2: SwiftUI.UnitPoint) -> some SwiftUI.View { self }
    public func mask<A1>(alignment a0: SwiftUI.Alignment, _ a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func contentShape<A1>(_ a0: SwiftUI.ContentShapeKinds, _ a1: A1, eoFill a2: Swift.Bool) -> some SwiftUI.View where A1: SwiftUI.Shape { self }
    public func foregroundStyle<A1, B1>(_ a0: A1, _ a1: B1) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle, B1: SwiftUI.ShapeStyle { self }
    public func foregroundStyle<A1, B1, C1>(_ a0: A1, _ a1: B1, _ a2: C1) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle, B1: SwiftUI.ShapeStyle, C1: SwiftUI.ShapeStyle { self }
    public func containerShape<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.InsettableShape { self }
    public func navigationSplitViewStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.NavigationSplitViewStyle { self }
    public func glassEffect<A1>(_ a0: SwiftUI.Glass, `in` a1: A1) -> some SwiftUI.View where A1: SwiftUI.Shape { self }
    public func foregroundStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
    public func visualEffect<A1>(_ a0: @escaping @Sendable (SwiftUI.EmptyVisualEffect, SwiftUI.GeometryProxy) -> A1) -> some SwiftUI.View where A1: SwiftUI.VisualEffect { self }
    public func accentColor(_ a0: SwiftUI.Color?) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.accentColor, value: a0)) }
    public func colorEffect(_ a0: SwiftUI.Shader, isEnabled a1: Swift.Bool) -> some SwiftUI.View { self }
    public func distortionEffect(_ a0: SwiftUI.Shader, maxSampleOffset a1: CGSize, isEnabled a2: Swift.Bool) -> some SwiftUI.View { self }
    public func layerEffect(_ a0: SwiftUI.Shader, maxSampleOffset a1: CGSize, isEnabled a2: Swift.Bool) -> some SwiftUI.View { self }
    public func buttonBorderShape(_ a0: SwiftUI.ButtonBorderShape) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ButtonBorderShapeKey.self] = a0 }) }
    public func tabViewStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.TabViewStyle { self }
    public func focusEffectDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func scrollClipDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func scaleEffect(_ a0: CGSize, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.View { self }
    public func groupBoxStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.GroupBoxStyle { self }
    public func controlGroupStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ControlGroupStyle { self }
    public func datePickerStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.DatePickerStyle { self }
    public func tableStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.TableStyle { self }
    public func pickerStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.PickerStyle { self }
    public func listStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ListStyle { self }
    public func menuStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.MenuStyle { self }
    public func formStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.FormStyle { self }
    public func gaugeStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.GaugeStyle { self }
    public func disclosureGroupStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.DisclosureGroupStyle { self }
    public func glassEffectUnion<A1>(id a0: A1?, namespace a1: SwiftUI.Namespace.ID) -> some SwiftUI.View where A1: Swift.Hashable, A1: Swift.Sendable { self }
}
