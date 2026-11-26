// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation

// Foundation.SortDescriptor<Compared>: Reef's Foundation does not have it. The type lives in this
// module and the wrapper exports it under Foundation's mangled names (see ../README.md).
public struct SortDescriptor<Compared>: @unchecked Sendable {
    public var order: SortOrder
    let keyPath: PartialKeyPath<Compared>
    let comparator: (Compared, Compared) -> ComparisonResult

    public init<Value: Comparable>(_ keyPath: KeyPath<Compared, Value>, order: SortOrder = .forward) {
        self.order = order
        self.keyPath = keyPath
        self.comparator = { a, b in
            let l = a[keyPath: keyPath], r = b[keyPath: keyPath]
            return l < r ? .orderedAscending : (l == r ? .orderedSame : .orderedDescending)
        }
    }
}

extension SortDescriptor {
    public func compare(_ lhs: Compared, _ rhs: Compared) -> ComparisonResult {
        let r = comparator(lhs, rhs)
        if order == .forward { return r }
        return r == .orderedAscending ? .orderedDescending : (r == .orderedDescending ? .orderedAscending : .orderedSame)
    }
}
