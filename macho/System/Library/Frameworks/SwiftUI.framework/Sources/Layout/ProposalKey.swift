// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// ProposalKey (Layout).

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

struct ProposalKey {
    let width: CoreGraphics.CGFloat
    let height: CoreGraphics.CGFloat
}

extension SwiftUI.ProposalKey: Swift.Equatable {
    static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.ProposalKey: Swift.Hashable {
    func hash(into a0: inout Swift.Hasher) { _skeleton() }
}
