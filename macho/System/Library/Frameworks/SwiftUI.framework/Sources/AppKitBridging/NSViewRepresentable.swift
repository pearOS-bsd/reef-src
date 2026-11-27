// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSViewRepresentable (AppKitBridging).

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

public protocol NSViewRepresentable: SwiftUI.View where Self.Body == Swift.Never {
    associatedtype NSViewType: NSView
    associatedtype Coordinator
    func _overrideSizeThatFits(_ a0: inout CGSize, `in` a1: SwiftUI._ProposedSize, nsView a2: Self.NSViewType)
    func _overrideLayoutTraits(_ a0: inout SwiftUI._LayoutTraits, `for` a1: Self.NSViewType)
    static func _modifyBridgedViewInputs(_ a0: inout SwiftUI._ViewInputs)
    static func dismantleNSView(_ a0: Self.NSViewType, coordinator a1: Self.Coordinator)
    func _identifiedViewTree(`in` a0: Self.NSViewType) -> SwiftUI._IdentifiedViewTree
    func sizeThatFits(_ a0: SwiftUI.ProposedViewSize, nsView a1: Self.NSViewType, context a2: SwiftUI.NSViewRepresentableContext<Self>) -> CGSize?
    static var _invalidatesSizeOnConstraintChanges: Swift.Bool { get }
    static func _layoutOptions(_ a0: Self.NSViewType) -> SwiftUI._PlatformViewRepresentableLayoutOptions
    func makeNSView(context a0: SwiftUI.NSViewRepresentableContext<Self>) -> Self.NSViewType
    func updateNSView(_ a0: Self.NSViewType, context a1: SwiftUI.NSViewRepresentableContext<Self>)
    func makeCoordinator() -> Self.Coordinator
}

extension SwiftUI.NSViewRepresentable {
    public func _overrideSizeThatFits(_ a0: inout CGSize, `in` a1: SwiftUI._ProposedSize, nsView a2: Self.NSViewType) { _skeleton() }
    public func _overrideLayoutTraits(_ a0: inout SwiftUI._LayoutTraits, `for` a1: Self.NSViewType) { _skeleton() }
    public static func _modifyBridgedViewInputs(_ a0: inout SwiftUI._ViewInputs) { _skeleton() }
    public static func dismantleNSView(_ a0: Self.NSViewType, coordinator a1: Self.Coordinator) { _skeleton() }
    public func _identifiedViewTree(`in` a0: Self.NSViewType) -> SwiftUI._IdentifiedViewTree { _skeleton() }
    public func sizeThatFits(_ a0: SwiftUI.ProposedViewSize, nsView a1: Self.NSViewType, context a2: SwiftUI.NSViewRepresentableContext<Self>) -> CGSize? { _skeleton() }
    public static var _invalidatesSizeOnConstraintChanges: Swift.Bool { _skeleton() }
    public static func _layoutOptions(_ a0: Self.NSViewType) -> SwiftUI._PlatformViewRepresentableLayoutOptions { _skeleton() }
    public static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.NSViewRepresentable where Self.Coordinator == () {
    public func makeCoordinator() { _skeleton() }
}
