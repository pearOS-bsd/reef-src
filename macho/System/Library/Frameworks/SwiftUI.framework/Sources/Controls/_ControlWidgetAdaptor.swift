// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ControlWidgetAdaptor (Controls).

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

public struct _ControlWidgetAdaptor<A> where A: SwiftUI.ControlWidget {
    let control: A
    public init() { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    public init(_ a0: A) { _skeleton() }
}

extension SwiftUI._ControlWidgetAdaptor: SwiftUI.Widget {
    public typealias Body = Swift.Never
}
