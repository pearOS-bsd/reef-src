// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SubscriptionView (Views).

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

public struct SubscriptionView<A, B> where A: Combine.Publisher, B: SwiftUI.View, A.Failure == Swift.Never {
    public var content: B
    public var publisher: A
    public var action: (A.Output) -> ()
    public init(content a0: B, publisher a1: A, action a2: @escaping (A.Output) -> ()) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.SubscriptionView: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.SubscriptionView: SwiftUI.PrimitiveView {
}
