// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
import Calculate

// Drives CalculateExpression with key sequences and prints the observable state after every key.
// Built against our Calculate module; linked to the install name of Calculate.framework, so on the Mac it runs
// against Apple's real framework and on Reef against ours: the two outputs must be identical.
// usage: enginetest "1 2 + 3 =" ...        keys separated by spaces (names below)

typealias Op = CalculateExpression.Operation
let keys: [String: Op] = [
    "0": .digit0, "1": .digit1, "2": .digit2, "3": .digit3, "4": .digit4, "5": .digit5, "6": .digit6, "7": .digit7,
    "8": .digit8, "9": .digit9, ".": .decimal, "+": .add, "-": .subtract, "*": .multiply, "/": .divide, "=": .equals,
    "%": .percent, "AC": .allClear, "C": .clear, "neg": .negate, "del": .delete, "sqrt": .squareRoot, "sq": .square,
    "cube": .cube, "inv": .reciprocal, "fact": .factorial, "pi": .pi, "e": .eulerNumber, "ee": .ee, "(": .openParenthesis,
    ")": .closeParenthesis, "pow": .power, "sin": .sine, "cos": .cosine, "tan": .tangent, "ln": .logarithmNatural,
    "log": .logarithmBase10, "rand": .random, "mod": .mod, "and": .and, "or": .or, "xor": .xor, "nor": .nor, "not": .not,
    "A": .digitA, "B": .digitB, "C_": .digitC, "D": .digitD, "E": .digitE, "F": .digitF, "00": .digit00, "FF": .digitFF,
    "lsh": .leftShift, "rsh": .rightShift, "root": .root, "cuberoot": .cubeRoot, "ex": .exponentialBaseEuler,
    "10x": .exponentialBase10, "2x": .exponentialBase2, "log2": .logarithmBase2, "asin": .inverseSine, "acos": .inverseCosine,
    "atan": .inverseTangent, "sinh": .hyperbolicSine, "cosh": .hyperbolicCosine, "tanh": .hyperbolicTangent,
]

func state(_ e: CalculateExpression) -> String {
    let rich = e.rich.tokens.map { $0.text }.joined(separator: "|")
    let err = e.error.map { ($0 as NSError).code.description } ?? "-"
    return "expr=\(e.expression) rich=[\(rich)] result=\(e.result?.formattedResult ?? "nil") clear=\(e.prefersClear) err=\(err)"
}

func dump(_ x: Any, _ depth: Int, _ indent: String) {
    let m = Mirror(reflecting: x)
    for c in m.children {
        let label = c.label ?? "_"
        let v = c.value
        let sub = Mirror(reflecting: v)
        if depth > 0 && (sub.displayStyle == .class || sub.displayStyle == .struct || sub.displayStyle == .collection || sub.displayStyle == .optional || sub.displayStyle == .enum || sub.displayStyle == .tuple) {
            print("\(indent)\(label): \(type(of: v))")
            dump(v, depth - 1, indent + "  ")
        } else {
            print("\(indent)\(label) = \(v)")
        }
    }
}

var base = CalculateExpression.Base.base10
var args = Array(CommandLine.arguments.dropFirst())
if args.first == "-dump" {
    args.removeFirst()
    let e = CalculateExpression(options: [.assumeDegrees: true], base: base, id: UUID())
    for k in args[0].split(separator: " ").map(String.init) { if let op = keys[k] { _ = e.update(op) } }
    dump(e, 4, "")
    exit(0)
}
if args.first == "-base16" { base = .base16; args.removeFirst() }
for line in args {
    var deg = true
    let e = CalculateExpression(options: [.assumeDegrees: true], base: base, id: UUID())
    print("## \(line)")
    print("   start: \(state(e))")
    for k in line.split(separator: " ").map(String.init) {
        if k == "ie" { e.inlineEvaluate(); print("   ie | \(state(e))"); continue }
        if k == "rep" { e.repeatOperation(); print("   rep | \(state(e))"); continue }
        if k == "deg" { e.assumeDegrees = true; deg = true; continue }
        if k == "rad" { e.assumeDegrees = false; deg = false; continue }
        guard let op = keys[k] else { print("   ?? \(k)"); continue }
        let ok = e.update(op)
        print("   \(k) -> \(ok) | \(state(e))")
    }
    _ = deg
}
