// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NSApplicationDelegateAdaptor (AppKitBridging).

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

public struct NSApplicationDelegateAdaptor<A> where A: NSObject, A: NSApplicationDelegate {
    let box: SwiftUI.DelegateBox
    public init(_ a0: A.Type) { _skeleton() }
    public var wrappedValue: A { _skeleton() }
}

extension SwiftUI.NSApplicationDelegateAdaptor where A: Combine.ObservableObject {
    public init(_ a0: A.Type) { _skeleton() }
    public var projectedValue: SwiftUI.ObservedObject<A>.Wrapper { _skeleton() }
}

extension SwiftUI.NSApplicationDelegateAdaptor: SwiftUI.DynamicProperty {
}
