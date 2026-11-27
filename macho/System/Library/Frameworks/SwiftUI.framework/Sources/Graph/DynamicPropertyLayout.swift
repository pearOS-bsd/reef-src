// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// DynamicPropertyLayout (Graph).

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

/// Where the dynamic properties of a view type are (found once per type by reflection) and how each is brought up to
/// date before the body runs: properties that install themselves (State, Binding owners ...) get the body node and the
/// environment, other DynamicProperty values (and those with dynamic properties inside) get update().
enum DynamicPropertyLayout {
    /// install the dynamic properties of view a0 for node a1 (whose environment reads start again)
    static func install<A>(_ a0: inout A, node a1: SwiftUI.BodyNode, environment a2: SwiftUI.EnvironmentValues) {
        let entries = entries(of: A.self)
        if entries.isEmpty { return }
        a1.environmentReads = []
        withUnsafeMutableBytes(of: &a0) { raw in
            guard let base = raw.baseAddress else { return }
            for entry in entries {
                switch entry {
                case let .install(offset, type):
                    type._reefInstall(at: base + offset, context: SwiftUI.PropertyInstallContext(node: a1, environment: a2, index: offset))
                case let .update(offset, type):
                    update(type, at: base + offset)
                }
            }
        }
    }
    private static func update<P: SwiftUI.DynamicProperty>(_ a0: P.Type, at a1: Swift.UnsafeMutableRawPointer) {
        a1.assumingMemoryBound(to: P.self).pointee.update()
    }
    /// the entries of type a0 (cached; logged when tracing)
    static func entries(of a0: Any.Type) -> [SwiftUI.DynamicPropertyLayout.Entry] {
        if let known = cache[ObjectIdentifier(a0)] { return known }
        var entries: [SwiftUI.DynamicPropertyLayout.Entry] = []
        collect(a0, base: 0, into: &entries)
        cache[ObjectIdentifier(a0)] = entries
        if SwiftUI.ViewBuild.tracing, !entries.isEmpty {
            let lines = entries.map { entry -> String in
                switch entry {
                case let .install(offset, type): return "install " + String(reflecting: type) + "@" + offset.description
                case let .update(offset, type): return "update " + String(reflecting: type) + "@" + offset.description
                }
            }
            NSLog("SwiftUI: properties of %@: %@", String(reflecting: a0), lines.joined(separator: ", "))
        }
        return entries
    }
    /// the entries of a0 at byte offset a1: an installer is one entry; a dynamic property (or the view itself) is searched
    /// field by field, then a nested dynamic property gets an update entry after those of its fields
    static func collect(_ a0: Any.Type, base a1: Swift.Int, into a2: inout [SwiftUI.DynamicPropertyLayout.Entry]) {
        if let installer = a0 as? any SwiftUI.DynamicPropertyInstaller.Type {
            a2.append(.install(offset: a1, type: installer))
            return
        }
        if a0 is AnyClass || metadataKind(a0) == 0x303 { return }
        let property = a0 as? any SwiftUI.DynamicProperty.Type
        guard property != nil || a1 == 0 else { return }
        _ = forEachField(of: a0) { _, offset, type in
            if type is any SwiftUI.DynamicPropertyInstaller.Type || type is any SwiftUI.DynamicProperty.Type {
                collect(type, base: a1 + offset, into: &a2)
            }
            return true
        }
        if let property, a1 != 0 {
            a2.append(.update(offset: a1, type: property))
            if SwiftUI.ViewBuild.tracing {
                NSLog("SwiftUI: fields of %@ (size %d): %@", String(reflecting: a0), _openExistentialSize(a0), describeFields(a0))
            }
        }
    }
    private static func _openExistentialSize(_ a0: Any.Type) -> Swift.Int {
        func size<T>(_: T.Type) -> Int { MemoryLayout<T>.size }
        return _openExistential(a0, do: size)
    }
    /// "name@offset: Type" for each stored field of a0
    static func describeFields(_ a0: Any.Type) -> Swift.String {
        var fields: [String] = []
        _ = forEachField(of: a0) { name, offset, type in
            var field = String(cString: name)
            field.append("@")
            field.append(offset.description)
            field.append(": ")
            field.append(String(reflecting: type))
            fields.append(field)
            return true
        }
        return fields.joined(separator: ", ")
    }
    /// the kind word of a type's metadata (0x303: existential)
    private static func metadataKind(_ a0: Any.Type) -> Swift.Int {
        unsafeBitCast(a0, to: UnsafePointer<Swift.Int>.self).pointee
    }
    /// the stored fields of a struct or tuple type, as the standard library's _forEachField(of:options: .ignoreUnknown)
    /// gives them (its interface is not public, so through the runtime functions it calls)
    private static func forEachField(of a0: Any.Type, _ a1: (UnsafePointer<CChar>, Swift.Int, Any.Type) -> Swift.Bool) -> Swift.Bool {
        if a0 is AnyClass { return false }
        let count = _reefFieldCount(a0)
        if count <= 0 { return true }
        for index in 0..<count {
            let offset = _reefFieldOffset(a0, index)
            var field = SwiftUI._ReefFieldMetadata()
            let type = _reefFieldType(a0, index, &field)
            defer { field.freeFunc?(field.name) }
            guard let name = field.name else { continue }
            if !a1(name, offset, type) { return false }
        }
        return true
    }
    static var cache: [Swift.ObjectIdentifier: [SwiftUI.DynamicPropertyLayout.Entry]] = [:]
    enum Entry {
        case install(offset: Swift.Int, type: any SwiftUI.DynamicPropertyInstaller.Type)
        case update(offset: Swift.Int, type: any SwiftUI.DynamicProperty.Type)
    }
}

/// _FieldReflectionMetadata of the Swift runtime
struct _ReefFieldMetadata {
    var name: UnsafePointer<CChar>? = nil
    var freeFunc: (@convention(c) (UnsafePointer<CChar>?) -> Void)? = nil
    var isStrong: Swift.Bool = true
    var isVar: Swift.Bool = false
}
@_silgen_name("swift_reflectionMirror_recursiveCount")
func _reefFieldCount(_ a0: Any.Type) -> Swift.Int
@_silgen_name("swift_reflectionMirror_recursiveChildOffset")
func _reefFieldOffset(_ a0: Any.Type, _ a1: Swift.Int) -> Swift.Int
@_silgen_name("swift_reflectionMirror_recursiveChildMetadata")
func _reefFieldType(_ a0: Any.Type, _ a1: Swift.Int, _ a2: UnsafeMutablePointer<SwiftUI._ReefFieldMetadata>) -> Any.Type
