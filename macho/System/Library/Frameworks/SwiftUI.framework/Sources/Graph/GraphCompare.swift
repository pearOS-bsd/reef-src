// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// GraphCompare: whether two values (views, environment values) are the same, to know what to update.

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

    /// field by field through ViewFields and the bytes of the values; not written yet: every such value counts as changed
    static func viewEqual<A>(_ a: A, _ b: A) -> Swift.Bool { false }
}
