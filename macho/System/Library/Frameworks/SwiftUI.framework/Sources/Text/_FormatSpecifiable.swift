// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// _FormatSpecifiable (Text).

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

public protocol _FormatSpecifiable: Swift.Equatable {
    associatedtype _Arg: Swift.CVarArg
    var _specifier: Swift.String { get }
    var _arg: Self._Arg { get }
}
