// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SchemeAppearance: the color scheme of an AppKit appearance (dark when its name contains "Dark").

import AppKit

enum SchemeAppearance {
    static func scheme(of appearance: NSAppearance?) -> SwiftUI.ColorScheme {
        let name = appearance?.value(forKey: "name") as? String ?? ""
        return name.contains("Dark") ? .dark : .light
    }
}
