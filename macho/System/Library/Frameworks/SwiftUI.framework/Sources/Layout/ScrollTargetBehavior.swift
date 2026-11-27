// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ScrollTargetBehavior (Layout).

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

public protocol ScrollTargetBehavior {
    func properties(context a0: SwiftUI.ScrollTargetBehaviorPropertiesContext) -> SwiftUI.ScrollTargetBehaviorProperties
    static func _makeInputs(_ a0: SwiftUI._GraphValue<Self>, inputs a1: inout SwiftUI._ViewInputs)
    func _updateEnvironment(_ a0: inout SwiftUI.EnvironmentValues, context a1: SwiftUI._ScrollTargetBehaviorEnvironmentContext)
    func updateTarget(_ a0: inout SwiftUI.ScrollTarget, context a1: SwiftUI.ScrollTargetBehaviorContext)
}

extension SwiftUI.ScrollTargetBehavior {
    public func properties(context a0: SwiftUI.ScrollTargetBehaviorPropertiesContext) -> SwiftUI.ScrollTargetBehaviorProperties { _skeleton() }
    public static func _makeInputs(_ a0: SwiftUI._GraphValue<Self>, inputs a1: inout SwiftUI._ViewInputs) { _skeleton() }
    public func _updateEnvironment(_ a0: inout SwiftUI.EnvironmentValues, context a1: SwiftUI._ScrollTargetBehaviorEnvironmentContext) { _skeleton() }
}

extension SwiftUI.ScrollTargetBehavior where Self == SwiftUI.PagingScrollTargetBehavior {
    public static var paging: SwiftUI.PagingScrollTargetBehavior { _skeleton() }
}
