// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GaugeStyle (Drawing).

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

public protocol GaugeStyle {
}

extension SwiftUI.GaugeStyle where Self == SwiftUI.LinearCapacityGaugeStyle {
    public static var linearCapacity: SwiftUI.LinearCapacityGaugeStyle { _skeleton() }
}

extension SwiftUI.GaugeStyle where Self == SwiftUI.AccessoryCircularGaugeStyle {
    public static var accessoryCircular: SwiftUI.AccessoryCircularGaugeStyle { _skeleton() }
}

extension SwiftUI.GaugeStyle where Self == SwiftUI.AccessoryCircularCapacityGaugeStyle {
    public static var accessoryCircularCapacity: SwiftUI.AccessoryCircularCapacityGaugeStyle { _skeleton() }
}

extension SwiftUI.GaugeStyle where Self == SwiftUI.DefaultGaugeStyle {
    public static var automatic: SwiftUI.DefaultGaugeStyle { _skeleton() }
}
