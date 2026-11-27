// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ViewModifier (Views).

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

public protocol ViewModifier {
    associatedtype Body: SwiftUI.View
    static func _makeView(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs) -> SwiftUI._ViewOutputs
    static func _viewListCount(inputs a0: SwiftUI._ViewListCountInputs, body a1: (SwiftUI._ViewListCountInputs) -> Swift.Int?) -> Swift.Int?
    static func _makeViewList(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewListOutputs
    func body(content a0: SwiftUI._ViewModifier_Content<Self>) -> Self.Body
}

extension SwiftUI.ViewModifier where Self.Body == Swift.Never {
    public func body(content a0: SwiftUI._ViewModifier_Content<Self>) -> Swift.Never { _skeleton() }
}

extension SwiftUI.ViewModifier {
    public static func _makeView(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _viewListCount(inputs a0: SwiftUI._ViewListCountInputs, body a1: (SwiftUI._ViewListCountInputs) -> Swift.Int?) -> Swift.Int? { _skeleton() }
    public static func _makeViewList(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewListOutputs { _skeleton() }
}

extension SwiftUI.ViewModifier where Self: SwiftUI.Animatable {
    public static func _makeView(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewInputs) -> SwiftUI._ViewOutputs) -> SwiftUI._ViewOutputs { _skeleton() }
    public static func _makeViewList(modifier a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._ViewListInputs, body a2: @escaping (SwiftUI._Graph, SwiftUI._ViewListInputs) -> SwiftUI._ViewListOutputs) -> SwiftUI._ViewListOutputs { _skeleton() }
}
