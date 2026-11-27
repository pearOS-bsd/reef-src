// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// View: members of the Scenes area.

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

extension SwiftUI.View {
    public func fileExporter<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, document a1: A1?, contentTypes a2: [UniformTypeIdentifiers.UTType], defaultFilename a3: Swift.String?, onCompletion a4: @escaping (Swift.Result<Foundation.URL, any Swift.Error>) -> (), onCancellation a5: @escaping () -> ()) -> some SwiftUI.View where A1: SwiftUI.FileDocument { self }
    public func fileExporter<A1>(isPresented a0: SwiftUI.Binding<Swift.Bool>, item a1: A1?, contentTypes a2: [UniformTypeIdentifiers.UTType], defaultFilename a3: Swift.String?, onCompletion a4: @escaping (Swift.Result<Foundation.URL, any Swift.Error>) -> (), onCancellation a5: @escaping () -> ()) -> some SwiftUI.View where A1: CoreTransferable.Transferable { self }
    public func fileImporter(isPresented a0: SwiftUI.Binding<Swift.Bool>, allowedContentTypes a1: [UniformTypeIdentifiers.UTType], onCompletion a2: @escaping (Swift.Result<Foundation.URL, any Swift.Error>) -> ()) -> some SwiftUI.View { self }
    public func fileImporter(isPresented a0: SwiftUI.Binding<Swift.Bool>, allowedContentTypes a1: [UniformTypeIdentifiers.UTType], allowsMultipleSelection a2: Swift.Bool, onCompletion a3: @escaping (Swift.Result<[Foundation.URL], any Swift.Error>) -> ()) -> some SwiftUI.View { self }
    public func interactiveDismissDisabled(_ a0: Swift.Bool) -> some SwiftUI.View { self }
}
