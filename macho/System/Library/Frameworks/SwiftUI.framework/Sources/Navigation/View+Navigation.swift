// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Navigation area.

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
    func presentation(_ a0: SwiftUI.PresentationModifier.Kind) -> SwiftUI.ModifiedContent<Self, SwiftUI.PresentationModifier> { _skeleton() }
    public func tabItem<A1>(_ a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func contextMenu<A1>(_ a0: (SwiftUI.ContextMenu<A1>)?) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func windowMinimizeBehavior(_ a0: SwiftUI.WindowInteractionBehavior) -> some SwiftUI.View { self }
    public func popover<A1, B1>(item a0: SwiftUI.Binding<A1?>, attachmentAnchor a1: SwiftUI.PopoverAttachmentAnchor, arrowEdge a2: SwiftUI.Edge, content a3: @escaping (A1) -> B1) -> some SwiftUI.View where A1: Swift.Identifiable, B1: SwiftUI.View { self }
    public func popoverCore<A1, B1>(item a0: SwiftUI.Binding<A1?>, attachmentAnchor a1: SwiftUI.PopoverAttachmentAnchor, arrowEdges a2: SwiftUI.Edge.Set?, content a3: @escaping (A1) -> B1) -> some SwiftUI.View where A1: Swift.Identifiable, B1: SwiftUI.View { self }
    public func popoverCore<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, attachmentAnchor a1: SwiftUI.PopoverAttachmentAnchor, arrowEdges a2: SwiftUI.Edge.Set?, isDetachable a3: Swift.Bool, content a4: @escaping () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func alert<A1, B1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, error a1: A1?, actions a2: () -> B1) -> some SwiftUI.View where A1: Foundation.LocalizedError, B1: SwiftUI.View { self }
    public func navigationTitle(_ a0: SwiftUI.Text) -> some SwiftUI.View { self }
    public func fileDialogURLEnabled(_ a0: Foundation.Predicate<(Foundation.URL)>) -> some SwiftUI.View { self }
    public func navigationSplitViewColumnWidth(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func navigationSplitViewColumnWidth(min a0: CoreGraphics.CGFloat?, ideal a1: CoreGraphics.CGFloat, max a2: CoreGraphics.CGFloat?) -> some SwiftUI.View { self }
    public func searchable(text a0: SwiftUI.Binding<Swift.String>, placement a1: SwiftUI.SearchFieldPlacement, prompt a2: SwiftUI.Text?) -> some SwiftUI.View { self }
    public func searchable(text a0: SwiftUI.Binding<Swift.String>, placement a1: SwiftUI.SearchFieldPlacement, prompt a2: SwiftUI.LocalizedStringKey) -> some SwiftUI.View { self }
    public func searchable<A1>(text a0: SwiftUI.Binding<Swift.String>, placement a1: SwiftUI.SearchFieldPlacement, prompt a2: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { self }
    public func onDeleteCommand(perform a0: (() -> ())?) -> some SwiftUI.View { self }
    public func confirmationDialog<A1, B1>(_ a0: SwiftUI.Text, isPresented a1: SwiftUI.Binding<Swift.Bool>, titleVisibility a2: SwiftUI.Visibility, actions a3: () -> A1, message a4: () -> B1) -> some SwiftUI.View where A1: SwiftUI.View, B1: SwiftUI.View { self }
    public func confirmationDialog<A1, B1, C1>(_ a0: SwiftUI.Text, isPresented a1: SwiftUI.Binding<Swift.Bool>, titleVisibility a2: SwiftUI.Visibility, presenting a3: C1?, actions a4: (C1) -> A1, message a5: (C1) -> B1) -> some SwiftUI.View where A1: SwiftUI.View, B1: SwiftUI.View { self }
    public func alert<A1, B1, C1>(_ a0: SwiftUI.Text, isPresented a1: SwiftUI.Binding<Swift.Bool>, presenting a2: C1?, actions a3: (C1) -> A1, message a4: (C1) -> B1) -> some SwiftUI.View where A1: SwiftUI.View, B1: SwiftUI.View { self }
    public func alert<A1, B1, C1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, error a1: A1?, actions a2: (A1) -> B1, message a3: (A1) -> C1) -> some SwiftUI.View where A1: Foundation.LocalizedError, B1: SwiftUI.View, C1: SwiftUI.View { self }
    public func fileDialogMessage(_ a0: SwiftUI.Text?) -> some SwiftUI.View { self }
    public func fileDialogDefaultDirectory(_ a0: Foundation.URL?) -> some SwiftUI.View { self }
    public func fileDialogBrowserOptions(_ a0: SwiftUI.FileDialogBrowserOptions) -> some SwiftUI.View { self }
    public func inspector<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, content a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func inspectorColumnWidth(min a0: CoreGraphics.CGFloat?, ideal a1: CoreGraphics.CGFloat, max a2: CoreGraphics.CGFloat?) -> some SwiftUI.View { self }
    public func inspectorColumnWidth(_ a0: CoreGraphics.CGFloat) -> some SwiftUI.View { self }
    public func navigationSubtitle(_ a0: SwiftUI.LocalizedStringKey) -> some SwiftUI.View { self }
    public func navigationSubtitle<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { self }
    public func navigationSubtitle(_ a0: SwiftUI.Text) -> some SwiftUI.View { self }
    public func confirmationDialog<A1, B1>(_ a0: SwiftUI.LocalizedStringKey, isPresented a1: SwiftUI.Binding<Swift.Bool>, titleVisibility a2: SwiftUI.Visibility, actions a3: () -> A1, message a4: () -> B1) -> some SwiftUI.View where A1: SwiftUI.View, B1: SwiftUI.View { self }
    public func confirmationDialog<A1, B1, C1>(_ a0: A1, isPresented a1: SwiftUI.Binding<Swift.Bool>, titleVisibility a2: SwiftUI.Visibility, actions a3: () -> B1, message a4: () -> C1) -> some SwiftUI.View where A1: Swift.StringProtocol, B1: SwiftUI.View, C1: SwiftUI.View { self }
    public func alert(isPresented a0: SwiftUI.Binding<Swift.Bool>, content a1: () -> SwiftUI.Alert) -> some SwiftUI.View { self }
    public func alert<A1, B1>(_ a0: A1, isPresented a1: SwiftUI.Binding<Swift.Bool>, actions a2: () -> B1) -> some SwiftUI.View where A1: Swift.StringProtocol, B1: SwiftUI.View { self }
    public func alert<A1, B1, C1>(_ a0: A1, isPresented a1: SwiftUI.Binding<Swift.Bool>, actions a2: () -> B1, message a3: () -> C1) -> some SwiftUI.View where A1: Swift.StringProtocol, B1: SwiftUI.View, C1: SwiftUI.View { self }
    public func alert<A1, B1>(_ a0: SwiftUI.LocalizedStringKey, isPresented a1: SwiftUI.Binding<Swift.Bool>, actions a2: () -> A1, message a3: () -> B1) -> some SwiftUI.View where A1: SwiftUI.View, B1: SwiftUI.View { self }
    public func alert<A1, B1, C1, D1>(_ a0: A1, isPresented a1: SwiftUI.Binding<Swift.Bool>, presenting a2: D1?, actions a3: (D1) -> B1, message a4: (D1) -> C1) -> some SwiftUI.View where A1: Swift.StringProtocol, B1: SwiftUI.View, C1: SwiftUI.View { self }
    public func confirmationDialog<A1>(_ a0: SwiftUI.LocalizedStringKey, isPresented a1: SwiftUI.Binding<Swift.Bool>, titleVisibility a2: SwiftUI.Visibility, actions a3: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func sheet<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, onDismiss a1: (() -> ())?, content a2: @escaping () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func sheet<A1, B1>(item a0: SwiftUI.Binding<A1?>, onDismiss a1: (() -> ())?, content a2: @escaping (A1) -> B1) -> some SwiftUI.View where A1: Swift.Identifiable, B1: SwiftUI.View { self }
    public func popover<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, attachmentAnchor a1: SwiftUI.PopoverAttachmentAnchor, arrowEdge a2: SwiftUI.Edge, content a3: @escaping () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func presentationDetents(_ a0: Swift.Set<SwiftUI.PresentationDetent>) -> some SwiftUI.View { self }
    public func presentationDetents(_ a0: Swift.Set<SwiftUI.PresentationDetent>, selection a1: SwiftUI.Binding<SwiftUI.PresentationDetent>) -> some SwiftUI.View { self }
    public func toolbar<A1>(content a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.ToolbarContent { self }
    public func navigationTitle(_ a0: SwiftUI.LocalizedStringKey) -> some SwiftUI.View { self }
    public func navigationTitle<A1>(_ a0: A1) -> some SwiftUI.View where A1: Swift.StringProtocol { self }
    public func navigationDestination<A1, B1>(`for` a0: A1.Type, destination a1: @escaping (A1) -> B1) -> some SwiftUI.View where A1: Swift.Hashable, B1: SwiftUI.View { self }
    public func navigationDestination<A1, B1>(item a0: SwiftUI.Binding<A1?>, destination a1: @escaping (A1) -> B1) -> some SwiftUI.View where A1: Swift.Hashable, B1: SwiftUI.View { self }
    public func navigationDestination<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, destination a1: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func contextMenu<A1>(menuItems a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func contextMenu<A1, B1>(forSelectionType a0: A1.Type, menu a1: @escaping (Swift.Set<A1>) -> B1, primaryAction a2: ((Swift.Set<A1>) -> ())?) -> some SwiftUI.View where A1: Swift.Hashable, B1: SwiftUI.View { self }
    public func menuOrder(_ a0: SwiftUI.MenuOrder) -> some SwiftUI.View { self }
    public func onCommand(_ a0: ObjectiveC.Selector, perform a1: (() -> ())?) -> some SwiftUI.View { self }
    public func onExitCommand(perform a0: (() -> ())?) -> some SwiftUI.View { self }
    public func onMoveCommand(perform a0: ((SwiftUI.MoveCommandDirection) -> ())?) -> some SwiftUI.View { self }
    public func menuIndicator(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { self }
    public func tabViewSearchActivation(_ a0: SwiftUI.TabSearchActivation) -> some SwiftUI.View { self }
    public func tabViewSidebarHeader<A1>(content a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func tabViewSidebarFooter<A1>(content a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func tabViewSidebarBottomBar<A1>(content a0: () -> A1) -> some SwiftUI.View where A1: SwiftUI.View { self }
    public func tableColumnHeaders(_ a0: SwiftUI.Visibility) -> some SwiftUI.View { self }
    public func allowsWindowActivationEvents(_ a0: Swift.Bool?) -> some SwiftUI.View { self }
    public func navigationBarTitleDisplayMode(_ a0: SwiftUI.NavigationBarItem.TitleDisplayMode) -> some SwiftUI.View { self }
    public func toolbar(removing a0: SwiftUI.ToolbarDefaultItemKind?) -> some SwiftUI.View { self }
    public func presentationCompactAdaptation(_ a0: SwiftUI.PresentationAdaptation) -> some SwiftUI.View { self }
    public func presentationContentInteraction(_ a0: SwiftUI.PresentationContentInteraction) -> some SwiftUI.View { self }
    public func toolbarTitleDisplayMode(_ a0: SwiftUI.ToolbarTitleDisplayMode) -> some SwiftUI.View { self }
    public func navigationBarBackButtonHidden(_ a0: Swift.Bool) -> some SwiftUI.View { self }
    public func presentationSizing<A1>(_ a0: A1) -> some SwiftUI.View where A1: SwiftUI.PresentationSizing { self }
}
