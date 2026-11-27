// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DatePickerComponents (Controls).

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

public struct DatePickerComponents {
    public let rawValue: Swift.UInt
    public static var hourAndMinute: SwiftUI.DatePickerComponents { _skeleton() }
    public static var date: SwiftUI.DatePickerComponents { _skeleton() }
    public static var hourMinuteAndSecond: SwiftUI.DatePickerComponents { _skeleton() }
    public init(rawValue a0: Swift.UInt) { _skeleton() }
}

extension SwiftUI.DatePickerComponents: Swift.OptionSet {
    public typealias Element = SwiftUI.DatePickerComponents
}

extension SwiftUI.DatePickerComponents: Swift.SetAlgebra {
}

extension SwiftUI.DatePickerComponents: Swift.RawRepresentable {
    public typealias RawValue = Swift.UInt
}

extension SwiftUI.DatePickerComponents: Swift.Equatable {
    public static func == (a0: Self, a1: Self) -> Swift.Bool { _skeleton() }
}

extension SwiftUI.DatePickerComponents: Swift.ExpressibleByArrayLiteral {
    public typealias ArrayLiteralElement = SwiftUI.DatePickerComponents
}
