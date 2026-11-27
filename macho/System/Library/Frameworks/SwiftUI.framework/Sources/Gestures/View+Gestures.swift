// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Gestures area.

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
    public func onDrop(of a0: [UniformTypeIdentifiers.UTType], delegate a1: any SwiftUI.DropDelegate) -> some SwiftUI.View { self }
    public func onDrop(of a0: [UniformTypeIdentifiers.UTType], isTargeted a1: (SwiftUI.Binding<Swift.Bool>)?, perform a2: @escaping ([NSItemProvider]) -> Swift.Bool) -> some SwiftUI.View { self }
    public func onDrop(of a0: [UniformTypeIdentifiers.UTType], isTargeted a1: (SwiftUI.Binding<Swift.Bool>)?, perform a2: @escaping ([NSItemProvider], CGPoint) -> Swift.Bool) -> some SwiftUI.View { self }
    public func onHover(perform a0: @escaping (Swift.Bool) -> ()) -> some SwiftUI.View { self }
    public func hueRotation(_ a0: SwiftUI.Angle) -> some SwiftUI.View { self }
    public func rotation3DEffect(_ a0: SwiftUI.Angle, axis a1: (x: CoreGraphics.CGFloat, y: CoreGraphics.CGFloat, z: CoreGraphics.CGFloat), anchor a2: SwiftUI.UnitPoint, anchorZ a3: CoreGraphics.CGFloat, perspective a4: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func rotationEffect(_ a0: SwiftUI.Angle, anchor a1: SwiftUI.UnitPoint) -> some SwiftUI.View { self }
    public func onTapGesture(count a0: Swift.Int, perform a1: @escaping () -> ()) -> some SwiftUI.View { self }
    public func onLongPressGesture(minimumDuration a0: Swift.Double, maximumDistance a1: CoreGraphics.CGFloat, pressing a2: ((Swift.Bool) -> ())?, perform a3: @escaping () -> ()) -> some SwiftUI.View { self }
    public func dropDestination<A1>(`for` a0: A1.Type, action a1: @escaping ([A1], CGPoint) -> Swift.Bool, isTargeted a2: @escaping (Swift.Bool) -> ()) -> some SwiftUI.View where A1: CoreTransferable.Transferable { self }
    public func onDrag<A1>(_ a0: @escaping () -> NSItemProvider, preview a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func hoverEffect<A1>(_ a0: A1, isEnabled a1: Swift.Bool) -> some SwiftUI.View where A1: SwiftUI.CustomHoverEffect { self }
    public func draggable<A1>(containerItemID a0: A1, containerNamespace a1: SwiftUI.Namespace.ID?) -> some SwiftUI.View where A1: Swift.Hashable, A1: Swift.Sendable { self }
    public func draggable<A1>(_ a0: A1.Type, containerNamespace a1: SwiftUI.Namespace.ID?, _ a2: @escaping () -> A1?) -> some SwiftUI.View where A1: CoreTransferable.Transferable, A1: Swift.Identifiable, A1.ID: Swift.Sendable { self }
    public func draggable<A1>(_ a0: A1.Type, item a1: @escaping @autoclosure () -> A1?, containerNamespace a2: SwiftUI.Namespace.ID?) -> some SwiftUI.View where A1: CoreTransferable.Transferable, A1: Swift.Identifiable, A1.ID: Swift.Sendable { self }
    public func dragContainer<A1, B1>(`for` a0: A1.Type, `in` a1: SwiftUI.Namespace.ID?, _ a2: @escaping ([A1.ID]) -> B1) -> some SwiftUI.View where A1: CoreTransferable.Transferable, A1: Swift.Identifiable, A1 == B1.Element, B1: Swift.Collection, A1.ID: Swift.Sendable { self }
    public func dropDestination<A1>(`for` a0: A1.Type, isEnabled a1: Swift.Bool, action a2: @escaping ([A1], SwiftUI.DropSession) -> ()) -> some SwiftUI.View where A1: CoreTransferable.Transferable { self }
    public func pointerStyle(_ a0: SwiftUI.PointerStyle?) -> some SwiftUI.View { self }
    public func onTapGesture<A1>(count a0: Swift.Int, coordinateSpace a1: A1, perform a2: @escaping (CGPoint) -> ()) -> some SwiftUI.View where A1: SwiftUI.CoordinateSpaceProtocol { self }
    public func onContinuousHover<A1>(coordinateSpace a0: A1, perform a1: @escaping (SwiftUI.HoverPhase) -> ()) -> some SwiftUI.View where A1: SwiftUI.CoordinateSpaceProtocol { self }
    public func onContinuousHover(coordinateSpace a0: SwiftUI.CoordinateSpace, perform a1: @escaping (SwiftUI.HoverPhase) -> ()) -> some SwiftUI.View { self }
    public func highPriorityGesture<A1>(_ a0: A1, including a1: SwiftUI.GestureMask) -> some SwiftUI.View where A1: SwiftUI.Gesture { self }
    public func simultaneousGesture<A1>(_ a0: A1, including a1: SwiftUI.GestureMask) -> some SwiftUI.View where A1: SwiftUI.Gesture { self }
    public func onDrag(_ a0: @escaping () -> NSItemProvider) -> some SwiftUI.View { self }
    public func swipeActions<A1>(edge a0: SwiftUI.HorizontalEdge, allowsFullSwipe a1: Swift.Bool, content a2: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func dragPreviewsFormation(_ a0: SwiftUI.DragDropPreviewsFormation) -> some SwiftUI.View { self }
    public func presentationDragIndicator(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { self }
    public func dragConfiguration(_ a0: SwiftUI.DragConfiguration) -> some SwiftUI.View { self }
    public func gesture<A1>(_ a0: A1, including a1: SwiftUI.GestureMask) -> some SwiftUI.View where A1: SwiftUI.Gesture { self }
}
