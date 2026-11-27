// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// CommandGroupParts (Navigation).

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

protocol CommandGroupParts {
    var placementName: Swift.String { get }
    var menuEntries: [SwiftUI.MenuEntry] { get }
    var positionValue: Swift.Int { get }
}
