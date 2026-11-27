// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityTraits (Environment).

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

public struct AccessibilityTraits {
    var traits: Swift.Set<Swift.Int>
    public static var isModal: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isSearchField: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var allowsDirectInteraction: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var causesPageTurn: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isTabBar: SwiftUI.AccessibilityTraits { _skeleton() }
    public init() { _skeleton() }
    public static var isButton: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isHeader: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isSelected: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isLink: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isImage: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var isStaticText: SwiftUI.AccessibilityTraits { _skeleton() }
    public static var updatesFrequently: SwiftUI.AccessibilityTraits { _skeleton() }
    public func union(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.AccessibilityTraits { _skeleton() }
    public func intersection(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.AccessibilityTraits { _skeleton() }
    public func symmetricDifference(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.AccessibilityTraits { _skeleton() }
    public func contains(_ a0: SwiftUI.AccessibilityTraits) -> Swift.Bool { _skeleton() }
    public func insert(_ a0: SwiftUI.AccessibilityTraits) -> (inserted: Swift.Bool, memberAfterInsert: SwiftUI.AccessibilityTraits) { _skeleton() }
    public func remove(_ a0: SwiftUI.AccessibilityTraits) -> SwiftUI.AccessibilityTraits? { _skeleton() }
    public func update(with a0: SwiftUI.AccessibilityTraits) -> SwiftUI.AccessibilityTraits? { _skeleton() }
    public func formUnion(_ a0: SwiftUI.AccessibilityTraits) { _skeleton() }
    public func formIntersection(_ a0: SwiftUI.AccessibilityTraits) { _skeleton() }
    public func formSymmetricDifference(_ a0: SwiftUI.AccessibilityTraits) { _skeleton() }
    public static func == (_ a0: SwiftUI.AccessibilityTraits, _ a1: SwiftUI.AccessibilityTraits) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.AccessibilityTraits: Swift.SetAlgebra {
    public typealias Element = SwiftUI.AccessibilityTraits
}

extension SwiftUI.AccessibilityTraits: Swift.Equatable {
}

extension SwiftUI.AccessibilityTraits: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.AccessibilityTraits
}
