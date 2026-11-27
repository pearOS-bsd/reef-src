// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// InspectorModifier (Navigation).

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

struct InspectorModifier<A> where A: SwiftUI.View {
    let isPresented: SwiftUI.Binding<Swift.Bool>
    let inspector: A
    var _width: SwiftUI.State<SwiftUI.InspectorColumnWidth?>
    init(isPresented a0: SwiftUI.Binding<Swift.Bool>, inspector a1: A) { _skeleton() }
    func body(content a0: SwiftUI._ViewModifier_Content<SwiftUI.InspectorModifier<A>>) -> some SwiftUI.View { _skeleton() }
}

extension SwiftUI.InspectorModifier: SwiftUI.ViewModifier {
}
