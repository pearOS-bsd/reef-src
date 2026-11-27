// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// NavigationLinkRow (Navigation).

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

struct NavigationLinkRow {
    let value: Swift.AnyHashable
    let push: () -> ()
    static var key: Swift.ObjectIdentifier { _skeleton() }
}

extension SwiftUI.NavigationLinkRow: SwiftUI.TagValueReading {
    var taggedHashable: Swift.AnyHashable? { _skeleton() }
}
