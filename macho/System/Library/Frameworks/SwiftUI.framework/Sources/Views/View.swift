// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View (Views).

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

public protocol View {
    associatedtype Body: SwiftUI.View
    static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs
    static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs
    static func _viewListCount(inputs a0: SwiftUI._ViewListCountInputs) -> Swift.Int?
    var body: Self.Body { get }
}

extension SwiftUI.View where Self.Body == Swift.Never {
    public var body: Swift.Never { _skeleton() }
}

extension SwiftUI.View where Self: Swift.Equatable {
    public func equatable() -> SwiftUI.EquatableView<Self> { _skeleton() }
}

extension SwiftUI.View where Self: SwiftUI.Animatable {
    public static func _makeView(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(view a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs { _skeleton() }
}
