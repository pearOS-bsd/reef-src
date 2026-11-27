// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PresentationModifier (Navigation).

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

struct PresentationModifier {
    let kind: SwiftUI.PresentationModifier.Kind
    func _apply(_ a0: SwiftUI.BuildContext, content a1: @escaping (SwiftUI.BuildContext) -> [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
    enum Kind {
        case sheet(isPresented: SwiftUI.Binding<Swift.Bool>, onDismiss: (() -> ())?, content: () -> SwiftUI.AnyView)
        case alert(isPresented: SwiftUI.Binding<Swift.Bool>, title: Swift.String, message: Swift.String?, actions: SwiftUI.AnyView, style: NSAlert.Style)
        case popover(isPresented: SwiftUI.Binding<Swift.Bool>, edge: SwiftUI.Edge, content: () -> SwiftUI.AnyView)
        case fileImporter(isPresented: SwiftUI.Binding<Swift.Bool>, types: [UniformTypeIdentifiers.UTType], multiple: Swift.Bool, completion: (Swift.Result<[Foundation.URL], any Swift.Error>) -> ())
        case fileExporter(isPresented: SwiftUI.Binding<Swift.Bool>, types: [UniformTypeIdentifiers.UTType], defaultName: Swift.String?, write: (Foundation.URL, UniformTypeIdentifiers.UTType?) async throws -> (), completion: (Swift.Result<Foundation.URL, any Swift.Error>) -> (), cancellation: () -> ())
        case contextMenu(SwiftUI.AnyView?)
        case help(Swift.String)
        case title(Swift.String)
        case subtitle(Swift.String)
    }
}

extension SwiftUI.PresentationModifier: SwiftUI.PrimitiveModifier {
}

extension SwiftUI.PresentationModifier: SwiftUI.ViewModifier {
    typealias Body = Swift.Never
}
