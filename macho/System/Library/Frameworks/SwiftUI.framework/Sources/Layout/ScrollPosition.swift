// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollPosition (Layout).

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

public struct ScrollPosition {
    public var edge: SwiftUI.Edge?
    public var point: CGPoint?
    var identifier: (any Swift.Hashable)?
    var anchor: SwiftUI.UnitPoint?
    public static func == (_ a0: SwiftUI.ScrollPosition, _ a1: SwiftUI.ScrollPosition) -> Swift.Bool { _skeleton() }
    public init<A>(idType a0: A.Type, edge a1: SwiftUI.Edge) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
    public init<A>(idType a0: A.Type) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
    public init<A>(idType a0: A.Type, point a1: CGPoint) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
    public var isPositionedByUser: Swift.Bool { _skeleton() }
    public var viewID: (any Swift.Hashable & Swift.Sendable)? { _skeleton() }
    public func viewID<A>(type a0: A.Type) -> A? where A: Swift.Hashable { _skeleton() }
    public func scrollTo(edge a0: SwiftUI.Edge) { _skeleton() }
    public func scrollTo(point a0: CGPoint) { _skeleton() }
    public func scrollTo(x a0: CoreGraphics.CGFloat) { _skeleton() }
    public func scrollTo(y a0: CoreGraphics.CGFloat) { _skeleton() }
    public func scrollTo<A>(id a0: A, anchor a1: SwiftUI.UnitPoint?) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
    public init<A>(idType a0: A.Type, x a1: CoreGraphics.CGFloat) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
    public init<A>(idType a0: A.Type, y a1: CoreGraphics.CGFloat) where A: Swift.Hashable, A: Swift.Sendable { _skeleton() }
}

extension SwiftUI.ScrollPosition: Swift.Equatable {
}
