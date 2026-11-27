// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BuildContext (Navigation).

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

struct BuildContext {
    unowned var host: SwiftUI.ViewHost
    var scope: SwiftUI.BodyNode
    var path: [Swift.AnyHashable]
    var environment: SwiftUI.EnvironmentValues
    var canvas: SwiftUI.ElementCanvas?
    var modifierContents: [Swift.ObjectIdentifier: SwiftUI.ModifierContent]
    var styleLabels: [Swift.ObjectIdentifier: SwiftUI.ModifierContent]
    var stackAxis: SwiftUI.Axis?
    var container: SwiftUI.Container
    var scrollAxes: SwiftUI.Axis.Set
    func persistent<A>(_ a0: () -> A) -> A where A: AnyObject { _skeleton() }
    func observingDeferred<A>(_ a0: () -> A) -> A { _skeleton() }
}
