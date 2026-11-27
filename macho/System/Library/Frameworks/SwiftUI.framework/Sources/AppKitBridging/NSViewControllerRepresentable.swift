// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSViewControllerRepresentable (AppKitBridging).

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

public protocol NSViewControllerRepresentable: SwiftUI.View where Self.Body == Swift.Never {
    associatedtype NSViewControllerType: NSViewController
    associatedtype Coordinator
    func sizeThatFits(_ a0: SwiftUI.ProposedViewSize, nsViewController a1: Self.NSViewControllerType, context a2: SwiftUI.NSViewControllerRepresentableContext<Self>) -> CGSize?
    static func dismantleNSViewController(_ a0: Self.NSViewControllerType, coordinator a1: Self.Coordinator)
    func _identifiedViewTree(`in` a0: Self.NSViewControllerType) -> SwiftUI._IdentifiedViewTree
    static var _invalidatesSizeOnConstraintChanges: Swift.Bool { get }
    static func _layoutOptions(_ a0: Self.NSViewControllerType) -> SwiftUI._PlatformViewRepresentableLayoutOptions
    func makeCoordinator() -> Self.Coordinator
    func updateNSViewController(_ a0: Self.NSViewControllerType, context a1: SwiftUI.NSViewControllerRepresentableContext<Self>)
    func makeNSViewController(context a0: SwiftUI.NSViewControllerRepresentableContext<Self>) -> Self.NSViewControllerType
}

extension SwiftUI.NSViewControllerRepresentable {
    public func sizeThatFits(_ a0: SwiftUI.ProposedViewSize, nsViewController a1: Self.NSViewControllerType, context a2: SwiftUI.NSViewControllerRepresentableContext<Self>) -> CGSize? { _skeleton() }
    public static func dismantleNSViewController(_ a0: Self.NSViewControllerType, coordinator a1: Self.Coordinator) { _skeleton() }
    public func _identifiedViewTree(`in` a0: Self.NSViewControllerType) -> SwiftUI._IdentifiedViewTree { _skeleton() }
    public static var _invalidatesSizeOnConstraintChanges: Swift.Bool { _skeleton() }
    public static func _layoutOptions(_ a0: Self.NSViewControllerType) -> SwiftUI._PlatformViewRepresentableLayoutOptions { _skeleton() }
    public static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.NSViewControllerRepresentable where Self.Coordinator == () {
    public func makeCoordinator() { _skeleton() }
}
