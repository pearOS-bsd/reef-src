// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TimelineScheduleMode (Misc).

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

public enum TimelineScheduleMode {
    case normal
    case lowFrequency
    public func hash(into a0: inout Swift.Hasher) { _skeleton() }
    public var hashValue: Swift.Int { _skeleton() }
    public static func == (_ a0: SwiftUI.TimelineScheduleMode, _ a1: SwiftUI.TimelineScheduleMode) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.TimelineScheduleMode: Swift.Equatable {
}

extension SwiftUI.TimelineScheduleMode: Swift.Hashable {
}
