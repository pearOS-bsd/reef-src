// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FileDialogBrowserOptions (Navigation).

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

public struct FileDialogBrowserOptions {
    public let rawValue: Swift.Int
    public init(rawValue a0: Swift.Int) { rawValue = a0 }
    public static var enumeratePackages: SwiftUI.FileDialogBrowserOptions { _skeleton() }
    public static var includeHiddenFiles: SwiftUI.FileDialogBrowserOptions { _skeleton() }
    public static var displayFileExtensions: SwiftUI.FileDialogBrowserOptions { _skeleton() }
}

extension SwiftUI.FileDialogBrowserOptions: Swift.OptionSet {
    public typealias Element = SwiftUI.FileDialogBrowserOptions
}

extension SwiftUI.FileDialogBrowserOptions: Swift.SetAlgebra {
}

extension SwiftUI.FileDialogBrowserOptions: Swift.RawRepresentable {
    public typealias RawValue = Swift.Int
}

extension SwiftUI.FileDialogBrowserOptions: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { a0.rawValue == a1.rawValue }
}

extension SwiftUI.FileDialogBrowserOptions: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.FileDialogBrowserOptions
}
