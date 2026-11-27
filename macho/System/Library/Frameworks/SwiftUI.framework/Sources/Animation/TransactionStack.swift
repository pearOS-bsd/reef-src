// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// TransactionStack: the transaction of the change being made (withTransaction) and the storage of transaction values.

enum TransactionStack {
    static var current = SwiftUI.Transaction()

    /// a property list with one value changed (values are copied: a transaction is a value)
    static func setting(_ value: Any?, for key: Swift.ObjectIdentifier, in list: SwiftUI.PropertyList) -> SwiftUI.PropertyListElement {
        var values = (list.elements as? SwiftUI.TransactionValues)?.values ?? [:]
        values[key] = value
        return SwiftUI.TransactionValues(values)
    }
}
