// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause
// Debug aid for the Mac VM: logs every LayoutSubview.sizeThatFits call that an app makes (proposal and answer),
// by interposing the symbol (tab.c). Used to compare Apple's SwiftUI with Reef's.
import SwiftUI
import Foundation

@_silgen_name("$s7SwiftUI13LayoutSubviewV12sizeThatFitsySo6CGSizeVAA16ProposedViewSizeVF")
func origSTF(_ p: ProposedViewSize, _ s: LayoutSubview) -> CGSize

@_silgen_name("reef_my_stf")
func myStf(_ p: ProposedViewSize, _ s: LayoutSubview) -> CGSize {
    let r = origSTF(p, s)
    FileHandle.standardError.write("layout: sizeThatFits(\(String(describing: p.width)), \(String(describing: p.height))) -> \(r.width) x \(r.height)\n".data(using: .utf8)!)
    return r
}
