import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            GeometryReader { g in
                Text("size \(Int(g.size.width))x\(Int(g.size.height))").frame(width: g.size.width, height: g.size.height).background(Color.yellow.opacity(0.3))
                    .onAppear { Harness.note("gr1 size=\(g.size) global=\(g.frame(in: .global)) local=\(g.frame(in: .local)) insets=\(g.safeAreaInsets)") }
            }.frame(width: 200, height: 40).probe("gr1")
            GeometryReader { g in
                Color.red.frame(width: g.size.width / 2, height: g.size.height / 2).position(x: g.size.width / 2, y: g.size.height / 2)
            }.frame(height: 60).probe("gr2")
            GeometryReader { g in
                Text("unsized").onAppear { Harness.note("gr3 size=\(g.size)") }
            }.probe("gr3")
            HStack(spacing: 0) {
                GeometryReader { g in Color.green.onAppear { Harness.note("half1 \(g.size)") } }
                GeometryReader { g in Color.blue.onAppear { Harness.note("half2 \(g.size)") } }
            }.frame(width: 200, height: 30).probe("halves")
            ZStack {
                Color.gray.frame(width: 100, height: 40).coordinateSpace(name: "space").overlay(GeometryReader { g in Color.clear.onAppear { Harness.note("named \(g.frame(in: .named("space"))) global \(g.frame(in: .global))") } })
            }.padding(.leading, 30).probe("coords")
        }
        .frame(width: 420, height: 300)
        .finalState("geometryreader")
    }
}

Harness.run("geometryreader", 420, 300) { V() }
