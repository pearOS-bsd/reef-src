// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DatePickerStyle (Drawing).

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

public protocol DatePickerStyle {
}

extension SwiftUI.DatePickerStyle where Self == SwiftUI.StepperFieldDatePickerStyle {
    public static var stepperField: SwiftUI.StepperFieldDatePickerStyle { _skeleton() }
}

extension SwiftUI.DatePickerStyle where Self == SwiftUI.FieldDatePickerStyle {
    public static var field: SwiftUI.FieldDatePickerStyle { _skeleton() }
}

extension SwiftUI.DatePickerStyle where Self == SwiftUI.CompactDatePickerStyle {
    public static var compact: SwiftUI.CompactDatePickerStyle { _skeleton() }
}

extension SwiftUI.DatePickerStyle where Self == SwiftUI.DefaultDatePickerStyle {
    public static var automatic: SwiftUI.DefaultDatePickerStyle { _skeleton() }
}

extension SwiftUI.DatePickerStyle where Self == SwiftUI.GraphicalDatePickerStyle {
    public static var graphical: SwiftUI.GraphicalDatePickerStyle { _skeleton() }
}
