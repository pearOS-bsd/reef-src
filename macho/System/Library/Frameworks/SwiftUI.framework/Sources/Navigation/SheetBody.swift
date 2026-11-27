// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SheetBody (Navigation).

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

struct SheetBody {
    let content: SwiftUI.AnyView
    var _actions: SwiftUI.State<SwiftUI.SheetActions>
}

extension SwiftUI.SheetBody: SwiftUI.View {
    typealias Body = SwiftUI.VStack<SwiftUI.TupleView<(SwiftUI.ModifiedContent<SwiftUI.ModifiedContent<SwiftUI.AnyView, SwiftUI._PreferenceActionModifier<SwiftUI.SheetActionsKey>>, SwiftUI._EnvironmentKeyWritingModifier<Swift.Bool>>, (SwiftUI.TupleView<(SwiftUI.Divider, SwiftUI.ModifiedContent<SwiftUI.HStack<SwiftUI.TupleView<(SwiftUI.ForEach<Swift.Range<Swift.Int>, Swift.Int, SwiftUI.AnyView>, SwiftUI.Spacer, SwiftUI.ForEach<Swift.Range<Swift.Int>, Swift.Int, SwiftUI.ModifiedContent<SwiftUI.AnyView, SwiftUI.EnvironmentSetModifier>>, SwiftUI.ForEach<Swift.Range<Swift.Int>, Swift.Int, SwiftUI.ModifiedContent<SwiftUI.AnyView, SwiftUI.EnvironmentSetModifier>>)>>, SwiftUI._PaddingLayout>)>)?)>>
    var body: Self.Body { _skeleton() }
}
