// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusedObject (Controls).

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

public struct FocusedObject<A> where A: Combine.ObservableObject {
    public init() { _skeleton() }
    public var wrappedValue: A? { _skeleton() }
}

extension SwiftUI.FocusedObject: SwiftUI.DynamicProperty {
}
