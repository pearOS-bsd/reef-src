// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// PressTarget (Misc).

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

protocol PressTarget: AnyObject {
    func contains(_ a0: CGPoint) -> Swift.Bool
    func begin(at a0: CGPoint, event a1: NSEvent)
    func drag(to a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent)
    func end(at a0: CGPoint, inside a1: Swift.Bool, event a2: NSEvent)
}
