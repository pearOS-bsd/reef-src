// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicProperty (Graph).

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

public protocol DynamicProperty {
    static func _makeProperty<A1>(`in` a0: inout SwiftUI._DynamicPropertyBuffer, container a1: SwiftUI._GraphValue<A1>, fieldOffset a2: Swift.Int, inputs a3: inout SwiftUI._GraphInputs)
    static var _propertyBehaviors: Swift.UInt32 { get }
    func update()
    static var _propertyStorageSize: Swift.Int? { get }
}

extension SwiftUI.DynamicProperty {
    public static func _makeProperty<A1>(`in` a0: inout SwiftUI._DynamicPropertyBuffer, container a1: SwiftUI._GraphValue<A1>, fieldOffset a2: Swift.Int, inputs a3: inout SwiftUI._GraphInputs) { _skeleton() }
    public static var _propertyBehaviors: Swift.UInt32 { _skeleton() }
    public func update() { _skeleton() }
    public static var _propertyStorageSize: Swift.Int? { _skeleton() }
}
