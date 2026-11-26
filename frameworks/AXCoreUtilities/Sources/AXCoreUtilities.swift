// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation

extension Locale.Region {
    public var icuDisplayName: String? {
        return Locale(identifier: "en").localizedString(forRegionCode: self.identifier)
    }
}
