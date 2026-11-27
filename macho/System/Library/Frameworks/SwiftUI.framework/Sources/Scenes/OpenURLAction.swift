// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// OpenURLAction (Scenes).

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

public struct OpenURLAction {
    let handler: (Foundation.URL) -> SwiftUI.OpenURLAction.Result
    public func callAsFunction(_ a0: Foundation.URL) { _skeleton() }
    public init(handler a0: @escaping (Foundation.URL) -> SwiftUI.OpenURLAction.Result) { _skeleton() }
    public struct Result {
        let rawValue: Swift.Int
        let url: Foundation.URL?
        public static var handled: SwiftUI.OpenURLAction.Result { _skeleton() }
        public static var discarded: SwiftUI.OpenURLAction.Result { _skeleton() }
        public static var systemAction: SwiftUI.OpenURLAction.Result { _skeleton() }
        public static func systemAction(_ a0: Foundation.URL) -> SwiftUI.OpenURLAction.Result { _skeleton() }
    }
}
