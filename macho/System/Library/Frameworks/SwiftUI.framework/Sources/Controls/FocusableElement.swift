// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// FocusableElement (Controls).

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

class FocusableElement: SwiftUI.WrapperElement {
    let isFocused: () -> Swift.Bool = _skeletonValue()
    let setFocused: (Swift.Bool) -> () = _skeletonValue()
    let key: SwiftUI.FocusKey? = _skeletonValue()
    let target: SwiftUI.NoPress = _skeletonValue()
    override func hitTest(_ a0: CGPoint) -> SwiftUI.Element? { _skeleton() }
    init(_ a0: SwiftUI.Element, key a1: SwiftUI.FocusKey?, isFocused a2: @escaping () -> Swift.Bool, setFocused a3: @escaping (Swift.Bool) -> ()) { _skeleton() }
    deinit {}
}

extension SwiftUI.FocusableElement: SwiftUI.PressableElement {
    var pressTarget: any SwiftUI.PressTarget { _skeleton() }
    var focusOnClick: Swift.Bool { _skeleton() }
}
