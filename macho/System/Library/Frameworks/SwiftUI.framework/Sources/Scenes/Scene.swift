// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Scene (Scenes).

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

public protocol Scene {
    associatedtype Body: SwiftUI.Scene
    static func _makeScene(scene a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._SceneInputs) -> SwiftUI._SceneOutputs
    var body: Self.Body { get }
}

extension SwiftUI.Scene {
    public static func _makeScene(scene a0: SwiftUI._GraphValue<Self>, inputs a1: SwiftUI._SceneInputs) -> SwiftUI._SceneOutputs { _skeleton() }
    public func defaultSize(width a0: CoreGraphics.CGFloat, height a1: CoreGraphics.CGFloat) -> some SwiftUI.Scene { self }
    func modifier<A1>(_ a0: A1) -> SwiftUI.ModifiedContent<Self, A1> where A1: SwiftUI._SceneModifier { _skeleton() }
    public func defaultSize(_ a0: CGSize) -> some SwiftUI.Scene { self }
    public func windowResizability(_ a0: SwiftUI.WindowResizability) -> some SwiftUI.Scene { self }
    public func windowStyle<A1>(_ a0: A1) -> some SwiftUI.Scene where A1: SwiftUI.WindowStyle { self }
    public func windowToolbarStyle<A1>(_ a0: A1) -> some SwiftUI.Scene where A1: SwiftUI.WindowToolbarStyle { self }
    public func menuBarExtraStyle<A1>(_ a0: A1) -> some SwiftUI.Scene where A1: SwiftUI.MenuBarExtraStyle { self }
    public func commands<A1>(content a0: () -> A1) -> some SwiftUI.Scene where A1: SwiftUI.Commands { self }
    public func commandsRemoved() -> some SwiftUI.Scene { self }
    public func environmentObject<A1>(_ a0: A1) -> some SwiftUI.Scene where A1: Combine.ObservableObject { self }
    public func environment<A1>(_ a0: Swift.WritableKeyPath<SwiftUI.EnvironmentValues, A1>, _ a1: A1) -> some SwiftUI.Scene { self }
    public func defaultPosition(_ a0: SwiftUI.UnitPoint) -> some SwiftUI.Scene { self }
    public func defaultLaunchBehavior(_ a0: SwiftUI.SceneLaunchBehavior) -> some SwiftUI.Scene { self }
    public func restorationBehavior(_ a0: SwiftUI.SceneRestorationBehavior) -> some SwiftUI.Scene { self }
    public func windowBackgroundDragBehavior(_ a0: SwiftUI.WindowInteractionBehavior) -> some SwiftUI.Scene { self }
    public func handlesExternalEvents(matching a0: Swift.Set<Swift.String>) -> some SwiftUI.Scene { self }
}
