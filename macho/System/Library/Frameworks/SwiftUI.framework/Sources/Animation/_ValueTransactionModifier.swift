// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _ValueTransactionModifier (Animation).

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

public struct _ValueTransactionModifier<A> where A: Swift.Equatable {
    public var value: A
    public var transform: (inout SwiftUI.Transaction) -> ()
    public init(value a0: A, transform a1: @escaping (inout SwiftUI.Transaction) -> ()) { _skeleton() }
}

extension SwiftUI._ValueTransactionModifier: SwiftUI.ViewModifier {
    public typealias Body = Swift.Never
}
