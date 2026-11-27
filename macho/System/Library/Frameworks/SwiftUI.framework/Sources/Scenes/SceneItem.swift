// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SceneItem (Scenes).

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

struct SceneItem {
    let kind: SwiftUI.SceneItem.Kind
    let id: Swift.String?
    let title: Swift.String?
    let valueType: Any.Type?
    let content: (SwiftUI.PresentedValue) -> SwiftUI.AnyView
    var options: SwiftUI.SceneOptions
    enum Kind {
        case menuBarExtra(label: SwiftUI.AnyView, isInserted: (SwiftUI.Binding<Swift.Bool>)?)
        case windowGroup
        case window
        case settings
    }
}
