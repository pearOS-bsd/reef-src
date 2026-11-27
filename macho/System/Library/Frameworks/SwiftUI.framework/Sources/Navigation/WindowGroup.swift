// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// WindowGroup (Navigation).

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

public struct WindowGroup<A> where A: SwiftUI.View {
    let title: SwiftUI.Text?
    let id: Swift.String?
    let valueType: Any.Type?
    let content: (SwiftUI.PresentedValue) -> A
    func sceneItems(_ a0: SwiftUI.SceneOptions) -> [SwiftUI.SceneItem] { _skeleton() }
    init(title a0: SwiftUI.Text?, id a1: Swift.String?, valueType a2: Any.Type?, content a3: @escaping (SwiftUI.PresentedValue) -> A) { _skeleton() }
    public init(id a0: Swift.String, content a1: @escaping () -> A) { _skeleton() }
    public init(content a0: @escaping () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.Text, id a1: Swift.String, content a2: @escaping () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.Text, content a1: @escaping () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, id a1: Swift.String, content a2: @escaping () -> A) { _skeleton() }
    public init(_ a0: SwiftUI.LocalizedStringKey, content a1: @escaping () -> A) { _skeleton() }
    public init<A1>(_ a0: A1, id a1: Swift.String, content a2: @escaping () -> A) where A1: Swift.StringProtocol { _skeleton() }
    public init<A1>(_ a0: A1, content a1: @escaping () -> A) where A1: Swift.StringProtocol { _skeleton() }
    public var body: Swift.Never { _skeleton() }
    init<A1, B1>(title a0: SwiftUI.Text?, id a1: Swift.String?, `for` a2: A1.Type, defaultValue a3: (() -> A1)?, content a4: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(`for` a0: A1.Type, content a1: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(id a0: Swift.String, `for` a1: A1.Type, content a2: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.Text, `for` a1: A1.Type, content a2: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.Text, id a1: Swift.String, `for` a2: A1.Type, content a3: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.LocalizedStringKey, `for` a1: A1.Type, content a2: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(_ a0: SwiftUI.LocalizedStringKey, id a1: Swift.String, `for` a2: A1.Type, content a3: @escaping (SwiftUI.Binding<A1?>) -> B1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, `for` a1: B1.Type, content a2: @escaping (SwiftUI.Binding<B1?>) -> C1) where A == SwiftUI.PresentedWindowContent<B1, C1>, A1: Swift.StringProtocol, B1: Swift.Decodable, B1: Swift.Encodable, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.String, `for` a2: B1.Type, content a3: @escaping (SwiftUI.Binding<B1?>) -> C1) where A == SwiftUI.PresentedWindowContent<B1, C1>, A1: Swift.StringProtocol, B1: Swift.Decodable, B1: Swift.Encodable, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(`for` a0: A1.Type, content a1: @escaping (SwiftUI.Binding<A1>) -> B1, defaultValue a2: @escaping () -> A1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1>(id a0: Swift.String, `for` a1: A1.Type, content a2: @escaping (SwiftUI.Binding<A1>) -> B1, defaultValue a3: @escaping () -> A1) where A == SwiftUI.PresentedWindowContent<A1, B1>, A1: Swift.Decodable, A1: Swift.Encodable, A1: Swift.Hashable, B1: SwiftUI.View { _skeleton() }
    public init<A1, B1, C1>(_ a0: A1, id a1: Swift.String, `for` a2: B1.Type, content a3: @escaping (SwiftUI.Binding<B1>) -> C1, defaultValue a4: @escaping () -> B1) where A == SwiftUI.PresentedWindowContent<B1, C1>, A1: Swift.StringProtocol, B1: Swift.Decodable, B1: Swift.Encodable, B1: Swift.Hashable, C1: SwiftUI.View { _skeleton() }
}

extension SwiftUI.WindowGroup: SwiftUI.PrimitiveScene {
}

extension SwiftUI.WindowGroup: SwiftUI.Scene {
    public typealias Body = Swift.Never
}
