// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// BuildContext (Navigation).

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

struct BuildContext {
    unowned var host: SwiftUI.ViewHost
    var scope: SwiftUI.BodyNode
    var path: [Swift.AnyHashable]
    var environment: SwiftUI.EnvironmentValues
    var canvas: SwiftUI.ElementCanvas?
    var modifierContents: [Swift.ObjectIdentifier: SwiftUI.ModifierContent]
    var styleLabels: [Swift.ObjectIdentifier: SwiftUI.ModifierContent]
    var stackAxis: SwiftUI.Axis?
    var container: SwiftUI.Container
    var scrollAxes: SwiftUI.Axis.Set
    func persistent<A>(_ a0: () -> A) -> A where A: AnyObject { scope.persistentObject(SwiftUI.BodyNode.Key(parts: path), a0) }
    func observingDeferred<A>(_ a0: () -> A) -> A {
        // a0 run under observation tracking once per body run; a change of what it read invalidates the body node on
        // the main queue (only for the latest registration)
        let observation = scope.persistentObject(SwiftUI.BodyNode.Key(parts: path + [AnyHashable("observing")])) {
            SwiftUI.DeferredObservation()
        }
        let node = scope
        if observation.isWaiting && observation.bodyRun == node.bodyRuns { return a0() }
        observation.isWaiting = true
        observation.bodyRun = node.bodyRuns
        observation.generation += 1
        let generation = observation.generation
        return withObservationTracking(a0) { [weak observation, weak node] in
            DispatchQueue.main.async {
                guard let observation, observation.generation == generation else { return }
                observation.isWaiting = false
                node?.invalidate(SwiftUI.Transaction())
            }
        }
    }
}
