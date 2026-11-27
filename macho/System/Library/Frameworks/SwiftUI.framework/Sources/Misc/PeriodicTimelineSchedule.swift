// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PeriodicTimelineSchedule (Misc).

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

public struct PeriodicTimelineSchedule {
    let start: Foundation.Date
    let interval: Swift.Double
    public init(from a0: Foundation.Date, by a1: Swift.Double) { _skeleton() }
    public func entries(from a0: Foundation.Date, mode a1: SwiftUI.TimelineScheduleMode) -> [Foundation.Date] { _skeleton() }
}

extension SwiftUI.PeriodicTimelineSchedule: SwiftUI.TimelineSchedule {
    public typealias Entries = [Foundation.Date]
}
