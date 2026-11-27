// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TimelineSchedule (Misc).

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

public protocol TimelineSchedule where Self.Entries.Element == Foundation.Date {
    associatedtype Entries: Swift.Sequence
    func entries(from a0: Foundation.Date, mode a1: SwiftUI.TimelineScheduleMode) -> Self.Entries
}

extension SwiftUI.TimelineSchedule where Self == SwiftUI.AnimationTimelineSchedule {
    public static var animation: SwiftUI.AnimationTimelineSchedule { _skeleton() }
    public static func animation(minimumInterval a0: Swift.Double?, paused a1: Swift.Bool) -> SwiftUI.AnimationTimelineSchedule { _skeleton() }
}

extension SwiftUI.TimelineSchedule where Self == SwiftUI.EveryMinuteTimelineSchedule {
    public static var everyMinute: SwiftUI.EveryMinuteTimelineSchedule { _skeleton() }
}

extension SwiftUI.TimelineSchedule where Self == SwiftUI.PeriodicTimelineSchedule {
    public static func periodic(from a0: Foundation.Date, by a1: Swift.Double) -> SwiftUI.PeriodicTimelineSchedule { _skeleton() }
}
