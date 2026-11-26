import SwiftUI

struct V: View {
    @State var step = 0
    @State var log: [String] = []
    var body: some View {
        VStack(spacing: 6) {
            Text("step \(step)").probe("step")
            Text(log.joined(separator: ",")).probe("log")
            if step >= 1 { Text("one").probe("one").onAppear { log.append("one"); DispatchQueue.main.async { step = 2 } } }
            if step >= 2 { Text("two").probe("two").onAppear { log.append("two"); DispatchQueue.main.async { step = 3 } } }
            if step >= 3 { Text("three").probe("three").onAppear { log.append("three"); Harness.note("chain done \(log)") } }
        }
        .frame(width: 360, height: 260)
        .onAppear { step = 1 }
        .onChange(of: step) { _, v in Harness.note("step \(v)") }
        .finalState("state_chain")
    }
}

Harness.run("state_chain", 360, 260) { V() }
