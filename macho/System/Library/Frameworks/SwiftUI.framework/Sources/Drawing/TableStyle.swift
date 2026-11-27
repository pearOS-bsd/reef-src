// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TableStyle (Drawing).

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

public protocol TableStyle {
}

extension SwiftUI.TableStyle where Self == SwiftUI.InsetTableStyle {
    public static var inset: SwiftUI.InsetTableStyle { _skeleton() }
}

extension SwiftUI.TableStyle where Self == SwiftUI.BorderedTableStyle {
    public static var bordered: SwiftUI.BorderedTableStyle { _skeleton() }
}

extension SwiftUI.TableStyle where Self == SwiftUI.AutomaticTableStyle {
    public static var automatic: SwiftUI.AutomaticTableStyle { _skeleton() }
}
