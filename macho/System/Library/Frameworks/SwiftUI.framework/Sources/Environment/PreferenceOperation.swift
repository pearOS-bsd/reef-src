// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PreferenceOperation (Environment).

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

struct PreferenceOperation {
    let key: Swift.ObjectIdentifier
    let kind: SwiftUI.PreferenceOperation.Kind
    enum Kind {
        case write((SwiftUI.Element) -> Any)
        case transform((inout Any, SwiftUI.Element) -> ())
    }
}
