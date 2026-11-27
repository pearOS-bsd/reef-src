// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TabPage (Navigation).

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

struct TabPage {
    let label: SwiftUI.AnyView?
    let tag: Swift.AnyHashable?
    let view: SwiftUI.AnyView
    var section: SwiftUI.TabSectionMark?
}
