// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Layout area.

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
    public func frame(minWidth a0: CoreGraphics.CGFloat?, idealWidth a1: CoreGraphics.CGFloat?, maxWidth a2: CoreGraphics.CGFloat?, minHeight a3: CoreGraphics.CGFloat?, idealHeight a4: CoreGraphics.CGFloat?, maxHeight a5: CoreGraphics.CGFloat?, alignment a6: SwiftUI.Alignment) -> some SwiftUI.View {
        modifier(SwiftUI._FlexFrameLayout(minWidth: a0, idealWidth: a1, maxWidth: a2, minHeight: a3, idealHeight: a4, maxHeight: a5, alignment: a6))
    }
    public func fixedSize(horizontal a0: Swift.Bool, vertical a1: Swift.Bool) -> some SwiftUI.View { self }
    public func matchedGeometryEffect<A1>(id a0: A1, `in` a1: SwiftUI.Namespace.ID, properties a2: SwiftUI.MatchedGeometryProperties, anchor a3: SwiftUI.UnitPoint, isSource a4: Swift.Bool) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func padding(_ a0: SwiftUI.EdgeInsets) -> some SwiftUI.View {
        modifier(SwiftUI._PaddingLayout(edges: .all, insets: a0))
    }
    public func padding(_ a0: SwiftUI.Edge.Set, _ a1: CoreGraphics.CGFloat?) -> some SwiftUI.View {
        modifier(SwiftUI._PaddingLayout(edges: a0, insets: a1.map { SwiftUI.EdgeInsets(_all: $0) }))
    }
    public func padding(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View {
        modifier(SwiftUI._PaddingLayout(edges: .all, insets: SwiftUI.EdgeInsets(_all: a0)))
    }
    public func frame(width a0: CoreGraphics.CGFloat?, height a1: CoreGraphics.CGFloat?, alignment a2: SwiftUI.Alignment) -> some SwiftUI.View {
        modifier(SwiftUI._FrameLayout(width: a0, height: a1, alignment: a2))
    }
    public func alignmentGuide(_ a0: SwiftUI.VerticalAlignment, computeValue a1: @escaping (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func multilineTextAlignment(_ a0: SwiftUI.TextAlignment) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.multilineTextAlignment, value: a0)) }
    public func lineSpacing(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI._EnvironmentKeyWritingModifier(keyPath: \.lineSpacing, value: a0)) }
    public func baselineOffset(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.TextBaselineOffsetKey.self] = a0 }) }
    public func ignoresSafeArea(_ a0: SwiftUI.SafeAreaRegions, edges a1: SwiftUI.Edge.Set) -> some SwiftUI.View { self }
    public func edgesIgnoringSafeArea(_ a0: SwiftUI.Edge.Set) -> some SwiftUI.View { self }
    public func overlay<A1>(_ a0: A1, alignment a1: SwiftUI.Alignment) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func backgroundStyle<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.BackgroundStyleBoxKey.self] = SwiftUI.ShapeStyleBox(a0, secondary: nil, tertiary: nil) }) }
    public func gridCellColumns(_ a0: Swift.Int) -> some SwiftUI.View { self }
    public func gridCellAnchor(_ a0: SwiftUI.UnitPoint) -> some SwiftUI.View { self }
    public func gridColumnAlignment(_ a0: SwiftUI.HorizontalAlignment) -> some SwiftUI.View { self }
    public func gridCellUnsizedAxes(_ a0: SwiftUI.Axis.Set) -> some SwiftUI.View { self }
    public func offset(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func offset(_ a0: CGSize) -> some SwiftUI.View { self }
    public func fixedSize() -> some SwiftUI.View { self }
    public func position(x a0: CoreGraphics.CGFloat, y a1: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func background<A1>(_ a0: A1, alignment a1: SwiftUI.Alignment) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func background<A1>(alignment a0: SwiftUI.Alignment, content a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func coordinateSpace<A1>(name a0: A1) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func alternatingRowBackgrounds(_ a0: SwiftUI.AlternatingRowBackgroundBehavior) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.AlternatesRowBackgroundsKey.self] = Optional(a0.rawValue == 1) }) }
    public func background<A1, B1>(_ a0: A1, `in` a1: B1, fillStyle a2: SwiftUI.FillStyle) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle, B1: SwiftUI.Shape { self }
    public func onGeometryChange<A1>(`for` a0: A1.Type, of a1: @escaping (SwiftUI.GeometryProxy) -> A1, action a2: @escaping (A1, A1) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func onGeometryChange<A1>(`for` a0: A1.Type, of a1: @escaping (SwiftUI.GeometryProxy) -> A1, action a2: @escaping (A1) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func scrollEdgeEffectStyle(_ a0: SwiftUI.ScrollEdgeEffectStyle?, `for` a1: SwiftUI.Edge.Set) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { var hard = $0[SwiftUI.HardScrollEdgesKey.self]; if let a0, a0.rawValue == 1 { hard.formUnion(a1) } else { hard.subtract(a1) }; $0[SwiftUI.HardScrollEdgesKey.self] = hard }) }
    public func safeAreaBar<A1>(edge a0: SwiftUI.VerticalEdge, alignment a1: SwiftUI.HorizontalAlignment, spacing a2: CoreGraphics.CGFloat?, content a3: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func containerCornerOffset(_ a0: SwiftUI.Edge.Set, sizeToFit a1: Swift.Bool) -> some SwiftUI.View { self }
    public func scrollPosition(_ a0: SwiftUI.Binding<SwiftUI.ScrollPosition>, anchor a1: SwiftUI.UnitPoint?) -> some SwiftUI.View { self }
    public func background<A1>(_ a0: A1, ignoresSafeAreaEdges a1: SwiftUI.Edge.Set) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
    public func labelIconToTitleSpacing(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.LabelIconToTitleSpacingKey.self] = Optional(a0) }) }
    public func scenePadding(_ a0: SwiftUI.Edge.Set) -> some SwiftUI.View { self }
    public func toolbarBackgroundVisibility(_ a0: SwiftUI.Visibility, `for` a1: SwiftUI.ToolbarPlacement...) -> some SwiftUI.View { self }
    public func aspectRatio(_ a0: CGSize, contentMode a1: SwiftUI.ContentMode) -> some SwiftUI.View { self }
    public func aspectRatio(_ a0: CoreGraphics.CGFloat?, contentMode a1: SwiftUI.ContentMode) -> some SwiftUI.View { self }
    public func containerRelativeFrame(_ a0: SwiftUI.Axis.Set, alignment a1: SwiftUI.Alignment) -> some SwiftUI.View { self }
    public func containerRelativeFrame(_ a0: SwiftUI.Axis.Set, alignment a1: SwiftUI.Alignment, _ a2: @escaping (CoreGraphics.CGFloat, SwiftUI.Axis) -> CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func defaultScrollAnchor(_ a0: SwiftUI.UnitPoint?) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ScrollAnchorsKey.self] = SwiftUI.ScrollAnchors(initialOffset: a0, sizeChanges: a0, alignment: a0) }) }
    public func layoutPriority(_ a0: Swift.Double) -> some SwiftUI.View { self }
    public func listRowInsets(_ a0: SwiftUI.EdgeInsets?) -> some SwiftUI.View { self }
    public func overlay<A1>(alignment a0: SwiftUI.Alignment, content a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func safeAreaInset<A1>(edge a0: SwiftUI.VerticalEdge, alignment a1: SwiftUI.HorizontalAlignment, spacing a2: CoreGraphics.CGFloat?, content a3: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func safeAreaInset<A1>(edge a0: SwiftUI.HorizontalEdge, alignment a1: SwiftUI.VerticalAlignment, spacing a2: CoreGraphics.CGFloat?, content a3: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func scrollContentBackground(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ScrollContentBackgroundKey.self] = a0 }) }
    public func scrollPosition<A1>(id a0: SwiftUI.Binding<A1?>, anchor a1: SwiftUI.UnitPoint?) -> some SwiftUI.View where A1: Swift.Hashable { self }
    public func previewLayout(_ a0: SwiftUI.PreviewLayout) -> some SwiftUI.View { self }
    public func layoutValue<A1>(key a0: A1.Type, value a1: A1.Value) -> some SwiftUI.View where A1: SwiftUI.LayoutValueKey { self }
    public func scrollTargetBehavior<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ScrollTargetBehavior { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.ScrollTargetBehaviorKey.self] = a0 }) }
    public func onScrollGeometryChange<A1>(`for` a0: A1.Type, of a1: @escaping (SwiftUI.ScrollGeometry) -> A1, action a2: @escaping (A1, A1) -> ()) -> some SwiftUI.View where A1: Swift.Equatable { self }
    public func defaultScrollAnchor(_ a0: SwiftUI.UnitPoint?, `for` a1: SwiftUI.ScrollAnchorRole) -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { var anchors = $0.scrollAnchors; switch a1.rawValue { case 0: anchors.initialOffset = a0; case 1: anchors.sizeChanges = a0; default: anchors.alignment = a0 }; $0[SwiftUI.ScrollAnchorsKey.self] = anchors }) }
    public func horizontalRadioGroupLayout() -> some SwiftUI.View { modifier(SwiftUI.EnvironmentSetModifier { $0[SwiftUI.HorizontalRadioGroupKey.self] = true }) }
    public func background<A1, B1>(_ a0: A1, `in` a1: B1, fillStyle a2: SwiftUI.FillStyle) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle, B1: SwiftUI.InsettableShape { self }
    public func containerBackground<A1>(`for` a0: SwiftUI.ContainerBackgroundPlacement, alignment a1: SwiftUI.Alignment, content a2: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func coordinateSpace(_ a0: SwiftUI.NamedCoordinateSpace) -> some SwiftUI.View { self }
    public func scrollTargetLayout(isEnabled a0: Swift.Bool) -> some SwiftUI.View { self }
    public func position(_ a0: CGPoint) -> some SwiftUI.View { self }
    public func alignmentGuide(_ a0: SwiftUI.HorizontalAlignment, computeValue a1: @escaping (SwiftUI.ViewDimensions) -> CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func presentationBackground<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
    public func listRowBackground<A1>(_ a0: A1?) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func containerBackground<A1>(_ a0: A1, `for` a1: SwiftUI.ContainerBackgroundPlacement) -> some SwiftUI.View where A1: SwiftUI.ShapeStyle { self }
}
