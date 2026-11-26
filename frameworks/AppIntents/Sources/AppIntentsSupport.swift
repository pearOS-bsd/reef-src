// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import AppIntents

// Default perform() of OpenIntent, absent from Reef's AppIntents.
extension OpenIntent {
    public func perform() async throws -> some IntentResult {
        return .result()
    }
}
