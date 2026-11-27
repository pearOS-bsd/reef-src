// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AccessibilityTechnologies (Accessibility).

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

public struct AccessibilityTechnologies {
    var technologySet: SwiftUI.AccessibilityTechnologySet
    public init() { _skeleton() }
    public static var voiceOver: SwiftUI.AccessibilityTechnologies { _skeleton() }
    public static var switchControl: SwiftUI.AccessibilityTechnologies { _skeleton() }
    public func union(_ a0: SwiftUI.AccessibilityTechnologies) -> SwiftUI.AccessibilityTechnologies { _skeleton() }
    public func intersection(_ a0: SwiftUI.AccessibilityTechnologies) -> SwiftUI.AccessibilityTechnologies { _skeleton() }
    public func symmetricDifference(_ a0: SwiftUI.AccessibilityTechnologies) -> SwiftUI.AccessibilityTechnologies { _skeleton() }
    public func contains(_ a0: SwiftUI.AccessibilityTechnologies) -> Swift.Bool { _skeleton() }
    public func insert(_ a0: SwiftUI.AccessibilityTechnologies) -> (inserted: Swift.Bool, memberAfterInsert: SwiftUI.AccessibilityTechnologies) { _skeleton() }
    public func remove(_ a0: SwiftUI.AccessibilityTechnologies) -> SwiftUI.AccessibilityTechnologies? { _skeleton() }
    public func update(with a0: SwiftUI.AccessibilityTechnologies) -> SwiftUI.AccessibilityTechnologies? { _skeleton() }
    public func formUnion(_ a0: SwiftUI.AccessibilityTechnologies) { _skeleton() }
    public func formIntersection(_ a0: SwiftUI.AccessibilityTechnologies) { _skeleton() }
    public func formSymmetricDifference(_ a0: SwiftUI.AccessibilityTechnologies) { _skeleton() }
    public static func == (_ a0: SwiftUI.AccessibilityTechnologies, _ a1: SwiftUI.AccessibilityTechnologies) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.AccessibilityTechnologies: Swift.SetAlgebra {
    public typealias Element = SwiftUI.AccessibilityTechnologies
}

extension SwiftUI.AccessibilityTechnologies: Swift.Equatable {
}

extension SwiftUI.AccessibilityTechnologies: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.AccessibilityTechnologies
}
