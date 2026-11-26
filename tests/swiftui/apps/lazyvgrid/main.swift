import SwiftUI

struct V: View {
    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 8) {
                LazyVGrid(columns: [GridItem(.fixed(60)), GridItem(.fixed(60)), GridItem(.fixed(60))], spacing: 4) {
                    ForEach(0..<7, id: \.self) { i in Color.blue.opacity(0.4).frame(height: 30).overlay(Text("\(i)")) }
                }.probe("fixed")
                LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible())], spacing: 4) {
                    ForEach(0..<5, id: \.self) { i in Color.green.opacity(0.4).frame(height: 24).overlay(Text("f\(i)")) }
                }.probe("flexible")
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 80))], spacing: 4) {
                    ForEach(0..<9, id: \.self) { i in Color.orange.opacity(0.4).frame(height: 24).overlay(Text("a\(i)")) }
                }.probe("adaptive")
                LazyVGrid(columns: [GridItem(.fixed(40), spacing: 20), GridItem(.flexible(minimum: 20, maximum: 100))], alignment: .leading, spacing: 10) {
                    ForEach(0..<4, id: \.self) { i in Color.pink.frame(height: 16) }
                }.probe("spacing")
                LazyHGrid(rows: [GridItem(.fixed(20)), GridItem(.fixed(20))], spacing: 4) {
                    ForEach(0..<6, id: \.self) { i in Color.purple.opacity(0.4).frame(width: 30) }
                }.probe("hgrid")
                LazyVStack(spacing: 2) { ForEach(0..<3, id: \.self) { Text("lazy \($0)") } }.probe("lazyvstack")
            }.padding(8)
        }
        .frame(width: 420, height: 380)
        .finalState("lazyvgrid")
    }
}

Harness.run("lazyvgrid", 420, 380) { V() }
