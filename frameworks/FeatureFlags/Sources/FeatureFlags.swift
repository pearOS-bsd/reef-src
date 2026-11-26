// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

public protocol FeatureFlagsKey {
    var domain: StaticString { get }
    var feature: StaticString { get }
}

public func isFeatureEnabled(_ key: any FeatureFlagsKey) -> Bool {
    return false
}
