// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphCompare: whether two values (views, environment values) are the same, to know what to update.

import Foundation

enum GraphCompare {
    /// Equatable values compare with ==; the others field by field (viewEqual)
    static func equal<A>(_ a: A, _ b: A) -> Swift.Bool {
        if let value = a as? any Swift.Equatable { return equatable(value, b) }
        return viewEqual(a, b)
    }

    private static func equatable<A: Swift.Equatable>(_ a: A, _ b: Any) -> Swift.Bool {
        guard let other = b as? A else { return false }
        return a == other
    }

    /// field by field through ViewFields: bindings by location and value, other fields by their bytes; the whole bytes
    /// when the layout of A is not fully known
    static func viewEqual<A>(_ a: A, _ b: A) -> Swift.Bool {
        let fields = SwiftUI.ViewFields.fields(of: A.self)
        guard fields.isComplete else { return bitwiseEqual(a, b) }
        return withUnsafeBytes(of: a) { left in
            withUnsafeBytes(of: b) { right in
                let l = left.baseAddress!, r = right.baseAddress!
                for entry in fields.entries {
                    if let binding = entry.binding {
                        if !binding.sameBinding(l + entry.offset, r + entry.offset) { return false }
                    } else if entry.size != 0, memcmp(l + entry.offset, r + entry.offset, entry.size) != 0 {
                        if SwiftUI.ViewBuild.tracing {
                            NSLog("SwiftUI: %@ differs at %d+%d", String(reflecting: A.self), entry.offset, entry.size)
                        }
                        return false
                    }
                }
                return true
            }
        }
    }

    /// the same bytes
    static func bitwiseEqual<A>(_ a: A, _ b: A) -> Swift.Bool {
        let size = MemoryLayout<A>.size
        if size == 0 { return true }
        return withUnsafeBytes(of: a) { left in
            withUnsafeBytes(of: b) { right in memcmp(left.baseAddress!, right.baseAddress!, size) == 0 }
        }
    }

    /// a0 and a1 of the same type and equal
    static func anyEqual(_ a0: Any?, _ a1: Any) -> Swift.Bool {
        guard let a0 else { return false }
        func open<T>(_ value: T) -> Swift.Bool {
            guard let other = a1 as? T else { return false }
            return equal(value, other)
        }
        return _openExistential(a0, do: open)
    }
}
