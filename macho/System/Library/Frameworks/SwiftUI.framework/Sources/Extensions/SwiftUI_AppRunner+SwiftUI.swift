// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// What SwiftUI adds to SwiftUI_AppRunner.

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

enum AppRunner {
    static func run<A>(_ a0: A.Type) where A: SwiftUI.App { _skeleton() }
    static func add(_ a0: NSMenu, to a1: NSMenu) { _skeleton() }
    static func mainMenu() -> NSMenu { _skeleton() }
    static func addStatusItem(label a0: SwiftUI.AnyView, item a1: SwiftUI.SceneItem) { _skeleton() }
    static func launchItems() -> [SwiftUI.SceneItem] { _skeleton() }
    static func `open`(_ a0: SwiftUI.SceneItem, value a1: Swift.AnyHashable?) -> NSWindow { _skeleton() }
    static func openWindow(id a0: Swift.String?, value a1: Swift.AnyHashable?) { _skeleton() }
    static func closeWindow(id a0: Swift.String) { _skeleton() }
    static func showSettings() { _skeleton() }
    static var delegate: NSObject? { _skeleton() }
    static var settingsWindow: NSWindow? { _skeleton() }
    static var items: [SwiftUI.SceneItem] { _skeleton() }
    static var windows: [NSWindow] { _skeleton() }
    static var windowIDs: [Swift.ObjectIdentifier: Swift.String] { _skeleton() }
    static var windowValues: [Swift.ObjectIdentifier: SwiftUI.PresentedValue] { _skeleton() }
    static var statusItems: [NSStatusItem] { _skeleton() }
    static var actions: SwiftUI.AppMenuActions { _skeleton() }
}
