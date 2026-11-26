// The scene API without title: WindowGroup(content:) (Reef's SwiftUI does not export this initialiser).
import SwiftUI

struct PlainApp: App {
    var body: some Scene {
        WindowGroup {
            Text("plain WindowGroup").frame(width: 300, height: 200).finalState("window_group_content")
        }
    }
}

PlainApp.main()
