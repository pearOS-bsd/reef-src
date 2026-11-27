// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TupleCommandContent (Navigation).

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

public struct TupleCommandContent<A> {
    let value: A
    var children: [Any] { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI.TupleCommandContent: SwiftUI.TupleCommandParts {
}

extension SwiftUI.TupleCommandContent: SwiftUI.Commands {
    public typealias Body = Swift.Never
}
