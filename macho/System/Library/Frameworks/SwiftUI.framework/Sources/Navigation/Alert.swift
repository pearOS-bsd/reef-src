// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Alert (Navigation).

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

public struct Alert {
    let title: SwiftUI.Text
    let message: SwiftUI.Text?
    let buttons: [SwiftUI.Alert.Button]
    public init(title a0: SwiftUI.Text, message a1: SwiftUI.Text?, dismissButton a2: SwiftUI.Alert.Button?) { _skeleton() }
    public init(title a0: SwiftUI.Text, message a1: SwiftUI.Text?, primaryButton a2: SwiftUI.Alert.Button, secondaryButton a3: SwiftUI.Alert.Button) { _skeleton() }
    public struct Button {
        let label: SwiftUI.Text?
        let action: (() -> ())?
        let role: SwiftUI.ButtonRole?
        public static func `default`(_ a0: SwiftUI.Text, action a1: (() -> ())?) -> SwiftUI.Alert.Button { _skeleton() }
        public static func cancel(_ a0: (() -> ())?) -> SwiftUI.Alert.Button { _skeleton() }
        public static func cancel(_ a0: SwiftUI.Text, action a1: (() -> ())?) -> SwiftUI.Alert.Button { _skeleton() }
        public static func destructive(_ a0: SwiftUI.Text, action a1: (() -> ())?) -> SwiftUI.Alert.Button { _skeleton() }
    }
}
