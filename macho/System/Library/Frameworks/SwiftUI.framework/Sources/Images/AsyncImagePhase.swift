// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AsyncImagePhase (Images).

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

public enum AsyncImagePhase {
    case success(SwiftUI.Image)
    case failure(any Swift.Error)
    case empty
    public var image: SwiftUI.Image? { _skeleton() }
    public var error: (any Swift.Error)? { _skeleton() }
}
