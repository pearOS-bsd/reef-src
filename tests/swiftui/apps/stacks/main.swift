import SwiftUI

struct StacksView: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack(alignment: .top, spacing: 4) {
                Rectangle().fill(.red).frame(width: 30, height: 30)
                Rectangle().fill(.green).frame(width: 40, height: 60)
                Rectangle().fill(.blue).frame(width: 50, height: 20)
            }
            HStack(alignment: .bottom) {
                Rectangle().fill(.red).frame(width: 30, height: 30)
                Rectangle().fill(.green).frame(width: 40, height: 60)
            }
            ZStack(alignment: .topTrailing) {
                Rectangle().fill(.gray).frame(width: 100, height: 100).probe("gray")
                Rectangle().fill(.orange).frame(width: 40, height: 40)
            }
        }
        .padding(20)
        .frame(width: 300, height: 300)
        .finalState("stacks")
    }
}

Harness.run("Stacks", 300, 300) { StacksView() }
