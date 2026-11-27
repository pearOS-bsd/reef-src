// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ShaderLibrary (Drawing).

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

public struct ShaderLibrary {
    let source: SwiftUI.ShaderLibrary.Source
    let key: Swift.String
    public static func bundle(_ a0: Bundle) -> SwiftUI.ShaderLibrary { _skeleton() }
    public static var `default`: SwiftUI.ShaderLibrary { _skeleton() }
    public init(data a0: Foundation.Data) { _skeleton() }
    public init(url a0: Foundation.URL) { _skeleton() }
    public static func == (_ a0: SwiftUI.ShaderLibrary, _ a1: SwiftUI.ShaderLibrary) -> Swift.Bool { _skeleton() }
    public subscript(dynamicMember a0: Swift.String) -> SwiftUI.ShaderFunction { _skeleton() }
    public static subscript(dynamicMember a0: Swift.String) -> SwiftUI.ShaderFunction { _skeleton() }
    enum Source {
        case bundle(Bundle)
        case url(Foundation.URL)
        case data(Foundation.Data)
    }
}

extension SwiftUI.ShaderLibrary: Swift.Equatable {
}
