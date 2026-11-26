// Starts through the SwiftUI App protocol (WindowGroup, Commands, Settings) instead of the AppKit host.
import SwiftUI

struct CommandsApp: App {
    @State var n = 0
    var body: some Scene {
        WindowGroup("Scene app") {
            VStack { Text("WindowGroup content \(n)").probe("content") }
                .frame(width: 400, height: 300)
                .onAppear { DispatchQueue.main.async { n = 1 } }
                .finalState("scene_app")
        }
        .commands {
            CommandMenu("Test") { Button("Do it") { }.keyboardShortcut("d") }
            CommandGroup(replacing: .newItem) { Button("Custom new") { } }
        }
        Settings { Text("Settings content").frame(width: 200, height: 100) }
    }
}

CommandsApp.main()
