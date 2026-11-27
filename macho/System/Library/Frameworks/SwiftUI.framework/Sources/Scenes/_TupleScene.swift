// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _TupleScene (Scenes).

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

public struct _TupleScene<A> {
    public var value: A
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    public init(_ a0: A) { _skeleton() }
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI._TupleScene: SwiftUI.PrimitiveScene {
}

extension SwiftUI._TupleScene: SwiftUI.Scene {
    public typealias Body = Swift.Never
}
