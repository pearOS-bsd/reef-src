// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableHeaderRowContent (Navigation).

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

public struct TableHeaderRowContent<A, B> where A: Swift.Identifiable, B: SwiftUI.View {
    let content: B
    public var tableRowBody: Swift.Never { _skeleton() }
    var headerView: SwiftUI.AnyView { _skeleton() }
    init(content a0: B) { _skeleton() }
}

extension SwiftUI.TableHeaderRowContent: SwiftUI.TableRowContent {
    public typealias TableRowValue = A
    public typealias TableRowBody = Swift.Never
}

extension SwiftUI.TableHeaderRowContent: SwiftUI.TableHeaderRowSource {
}
