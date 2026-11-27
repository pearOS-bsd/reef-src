// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// AnimationTimelineSchedule (Animation).

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

public struct AnimationTimelineSchedule {
    let minimumInterval: Swift.Double?
    let paused: Swift.Bool
    public func entries(from a0: Foundation.Date, mode a1: SwiftUI.TimelineScheduleMode) -> [Foundation.Date] { _skeleton() }
    public init(minimumInterval a0: Swift.Double?, paused a1: Swift.Bool) { _skeleton() }
}

extension SwiftUI.AnimationTimelineSchedule: SwiftUI.TimelineSchedule {
    public typealias Entries = [Foundation.Date]
}
