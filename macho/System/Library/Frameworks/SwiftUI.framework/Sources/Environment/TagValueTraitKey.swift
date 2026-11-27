// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TagValueTraitKey (Environment).

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

public struct TagValueTraitKey<A> where A: Swift.Hashable {
    public static var defaultValue: SwiftUI.TagValueTraitKey<A>.Value { _skeleton() }
    @frozen public enum Value {
        case tagged(A)
        case untagged
        var taggedHashable: Swift.AnyHashable? { _skeleton() }
    }
}

extension SwiftUI.TagValueTraitKey.Value: SwiftUI.TagValueReading {
}

extension SwiftUI.TagValueTraitKey: SwiftUI._ViewTraitKey {
}
