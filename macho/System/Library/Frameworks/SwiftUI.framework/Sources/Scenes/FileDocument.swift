// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FileDocument (Scenes).

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

public protocol FileDocument {
    static var writableContentTypes: [UniformTypeIdentifiers.UTType] { get }
    static var readableContentTypes: [UniformTypeIdentifiers.UTType] { get }
    init(configuration a0: SwiftUI.FileDocumentReadConfiguration) throws
    func fileWrapper(configuration a0: SwiftUI.FileDocumentWriteConfiguration) throws -> FileWrapper
}

extension SwiftUI.FileDocument {
    public static var writableContentTypes: [UniformTypeIdentifiers.UTType] { _skeleton() }
}
