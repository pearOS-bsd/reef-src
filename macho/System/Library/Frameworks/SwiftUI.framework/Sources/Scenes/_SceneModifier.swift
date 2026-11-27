// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _SceneModifier (Scenes).

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

public protocol _SceneModifier {
    associatedtype Body: SwiftUI.Scene
    func body(content a0: SwiftUI._SceneModifier_Content<Self>) -> Self.Body
}

extension SwiftUI._SceneModifier {
    public func body(content a0: SwiftUI._SceneModifier_Content<Self>) -> Swift.Never { _skeleton() }
}
