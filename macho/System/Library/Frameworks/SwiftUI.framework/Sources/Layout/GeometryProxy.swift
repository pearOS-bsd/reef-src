// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GeometryProxy (Layout).

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

public struct GeometryProxy {
    let frame: CGRect
    weak var element: SwiftUI.Element?
    public var size: CGSize { _skeleton() }
    public func frame(`in` a0: SwiftUI.CoordinateSpace) -> CGRect { _skeleton() }
    public func frame<A>(`in` a0: A) -> CGRect where A: SwiftUI.CoordinateSpaceProtocol { _skeleton() }
    public func bounds(of a0: SwiftUI.NamedCoordinateSpace) -> CGRect? { _skeleton() }
    public subscript<A>(_ a0: SwiftUI.Anchor<A>) -> A { _skeleton() }
    public var safeAreaInsets: SwiftUI.EdgeInsets { _skeleton() }
}
