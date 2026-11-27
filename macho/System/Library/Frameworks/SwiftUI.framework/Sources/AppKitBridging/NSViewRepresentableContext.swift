// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSViewRepresentableContext (AppKitBridging).

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

public struct NSViewRepresentableContext<A> where A: SwiftUI.NSViewRepresentable {
    public let coordinator: A.Coordinator
    public var transaction: SwiftUI.Transaction
    public var environment: SwiftUI.EnvironmentValues
    init(coordinator a0: A.Coordinator) { _skeleton() }
}
