// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentKeyOrigin: whether an environment key is the app's own (its type is not in the SwiftUI module).

enum EnvironmentKeyOrigin {
    private static var known: [Swift.ObjectIdentifier: Swift.Bool] = [:]

    static func isCustom(_ key: Any.Type) -> Swift.Bool {
        let id = ObjectIdentifier(key)
        if let custom = known[id] { return custom }
        let custom = !_typeName(key, qualified: true).hasPrefix("SwiftUI.")
        known[id] = custom
        return custom
    }
}
