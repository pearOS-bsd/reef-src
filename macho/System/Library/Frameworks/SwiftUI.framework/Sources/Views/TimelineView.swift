// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TimelineView (Views).

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

public struct TimelineView<A, B> where A: SwiftUI.TimelineSchedule {
    let schedule: A
    let content: (SwiftUI.TimelineView<A, B>.Context) -> B
    init(schedule a0: A, content a1: @escaping (SwiftUI.TimelineView<A, B>.Context) -> B) { _skeleton() }
    public struct Context {
        public let date: Foundation.Date
        public let cadence: SwiftUI.TimelineView<A, B>.Context.Cadence
        init(date a0: Foundation.Date, cadence a1: SwiftUI.TimelineView<A, B>.Context.Cadence) { _skeleton() }
        public enum Cadence {
            case live
            case seconds
            case minutes
            public static func < (_ a0: SwiftUI.TimelineView<A, B>.Context.Cadence, _ a1: SwiftUI.TimelineView<A, B>.Context.Cadence) -> Swift.Bool { _skeleton() }
            public static func == (_ a0: SwiftUI.TimelineView<A, B>.Context.Cadence, _ a1: SwiftUI.TimelineView<A, B>.Context.Cadence) -> Swift.Bool { _skeleton() }
            public func hash(into a0: inout Swift.Hasher) { a0.combine(Int(unsafeBitCast(self, to: UInt8.self))) }
            public var hashValue: Swift.Int { _hashValue(for: self) }
        }
    }
}

extension SwiftUI.TimelineView where B: SwiftUI.View {
    public init(_ a0: A, content a1: @escaping (SwiftUI.TimelineView<A, B>.Context) -> B) { _skeleton() }
    func _buildElements(_ a0: SwiftUI.BuildContext) -> [SwiftUI.Element] { _skeleton() }
}

extension SwiftUI.TimelineView.Context.Cadence: Swift.Equatable {
}

extension SwiftUI.TimelineView.Context.Cadence: Swift.Hashable {
}

extension SwiftUI.TimelineView.Context.Cadence: Swift.Comparable {
}

extension SwiftUI.TimelineView: SwiftUI.View where B: SwiftUI.View {
    public typealias Body = Swift.Never
}

extension SwiftUI.TimelineView: SwiftUI.PrimitiveView where B: SwiftUI.View {
}
