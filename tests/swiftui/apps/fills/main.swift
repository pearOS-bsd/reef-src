import SwiftUI

struct V: View {
    var body: some View {
        VStack(spacing: 8) {
            HStack(spacing: 8) {
                Rectangle().fill(LinearGradient(colors: [.red, .blue], startPoint: .leading, endPoint: .trailing)).frame(width: 80, height: 40).probe("linear")
                Rectangle().fill(RadialGradient(colors: [.yellow, .black], center: .center, startRadius: 0, endRadius: 30)).frame(width: 60, height: 60).probe("radial")
                Rectangle().fill(AngularGradient(colors: [.red, .green, .blue, .red], center: .center)).frame(width: 60, height: 60).probe("angular")
                Rectangle().fill(EllipticalGradient(colors: [.white, .purple])).frame(width: 60, height: 40).probe("elliptical")
            }
            HStack(spacing: 8) {
                Rectangle().fill(Color.red.opacity(0.3)).frame(width: 40, height: 40).probe("alpha")
                Rectangle().fill(Color(red: 0.2, green: 0.6, blue: 0.4)).frame(width: 40, height: 40).probe("rgb")
                Rectangle().fill(Color(hue: 0.6, saturation: 0.8, brightness: 0.9)).frame(width: 40, height: 40).probe("hsb")
                Rectangle().fill(.tint).frame(width: 40, height: 40).probe("tint")
                Rectangle().fill(.secondary).frame(width: 40, height: 40).probe("secondary")
                Rectangle().fill(.background).frame(width: 40, height: 40).border(.gray).probe("background")
            }
            HStack(spacing: 8) {
                Rectangle().fill(.blue).frame(width: 40, height: 40).shadow(color: .black.opacity(0.5), radius: 6, x: 3, y: 3).probe("shadow")
                Rectangle().fill(.red).frame(width: 40, height: 40).blur(radius: 4).probe("blur")
                Rectangle().fill(.red).frame(width: 40, height: 40).brightness(0.3).probe("brightness")
                Rectangle().fill(.red).frame(width: 40, height: 40).saturation(0).probe("saturation")
                Rectangle().fill(.red).frame(width: 40, height: 40).hueRotation(.degrees(120)).probe("hue")
                Rectangle().fill(.red).frame(width: 40, height: 40).colorMultiply(.green).probe("multiply")
            }
            HStack(spacing: 8) {
                Text("fgStyle").foregroundStyle(LinearGradient(colors: [.red, .blue], startPoint: .leading, endPoint: .trailing)).font(.title).probe("gradtext")
                Rectangle().fill(.ultraThinMaterial).frame(width: 50, height: 40).probe("material")
                Rectangle().fill(.black).frame(width: 40, height: 40).mask(Circle()).probe("mask")
                Rectangle().fill(.red).frame(width: 40, height: 40).blendMode(.multiply).probe("blend")
            }
        }
        .frame(width: 400, height: 360)
        .finalState("fills")
    }
}

Harness.run("fills", 400, 360) { V() }
