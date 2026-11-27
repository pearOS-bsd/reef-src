// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// Global functions and variables of the Misc area.

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

/// one element for a list of them: an empty element, the only one, or a centred column of them
func singleElement(_ a0: [SwiftUI.Element]) -> SwiftUI.Element {
    if a0.count == 1 { return a0[0] }
    if a0.isEmpty { return SwiftUI.EmptyElement() }
    return SwiftUI.StackElement(axis: .vertical, alignment: SwiftUI.HorizontalAlignment.center.key, spacing: nil, items: a0)
}
func isInsideLiftedPart(_ a0: SwiftUI.Element) -> Swift.Bool { _skeleton() }
func changeAction<A>(_ a0: SwiftUI.BuildContext, _ a1: A, initial a2: Swift.Bool, content a3: (SwiftUI.BuildContext) -> [SwiftUI.Element], _ a4: @escaping (A, A) -> ()) -> [SwiftUI.Element] where A: Swift.Equatable { _skeleton() }
func valueTask<A>(_ a0: SwiftUI.BuildContext, _ a1: A, priority a2: Swift.TaskPriority, content a3: (SwiftUI.BuildContext) -> [SwiftUI.Element], _ a4: @escaping () async -> ()) -> [SwiftUI.Element] where A: Swift.Equatable { _skeleton() }
func attach(_ a0: SwiftUI.PreferenceOperation, to a1: [SwiftUI.Element]) -> [SwiftUI.Element] { _skeleton() }
