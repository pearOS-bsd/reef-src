import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            ScrollView {
                VStack(spacing: 2) { ForEach(0..<30, id: \.self) { i in Text("Row \(i)").frame(maxWidth: .infinity).background(i % 2 == 0 ? Color.gray.opacity(0.2) : .clear) } }
                    .probe("content")
            }.frame(height: 120).probe("vertical")
            ScrollView(.horizontal) {
                HStack(spacing: 4) { ForEach(0..<20, id: \.self) { i in Text("C\(i)").padding(4).background(Color.blue.opacity(0.3)) } }.probe("hcontent")
            }.frame(height: 40).probe("horizontal")
            ScrollView(.horizontal, showsIndicators: false) {
                HStack { Color.red.frame(width: 300, height: 20) }
            }.frame(height: 30).probe("noindicators")
            ScrollView([.horizontal, .vertical]) {
                Color.green.frame(width: 500, height: 300)
            }.frame(width: 200, height: 80).probe("both")
            ScrollViewReader { proxy in
                ScrollView { VStack { ForEach(0..<20, id: \.self) { i in Text("Target \(i)").id(i) } } }
                    .frame(height: 60).probe("reader")
                    .onAppear { DispatchQueue.main.async { proxy.scrollTo(10, anchor: .top) } }
            }
        }
        .padding(8)
        .frame(width: 420, height: 360)
        .finalState("scrollview")
    }
}

Harness.run("scrollview", 420, 360) { V() }
