// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
@_exported import CalculateObjC

extension CalculateExpression {
    public enum Base: Int, Hashable {
        case base8 = 8
        case base10 = 10
        case base16 = 16
    }

    public enum TokenType: Hashable {
        case leftRotate, rightShift, eulerNumber, placeholder, rightRotate, exponentiate
        case openParenthesis, closeParenthesis, or, pi, add, neg, nor, not, xor, unit, comma
        case equal, divide, percent, function, multiply, subtract, factorial, leftShift
        case number, other
    }

    open class Token {
        public final var type: TokenType
        public final var text: String
        public init(type: TokenType, text: String) { self.type = type; self.text = text }
    }

    open class PlainExpression {
        open var tokens: [Token] = []
        public init() {}
    }

    open class RichToken {
        public final var type: TokenType = .other
        open var text: String = ""
        open var left: RichExpression? { nil }
        open var right: RichExpression? { nil }
        open var isLog: Bool { false }
        open var isRoot: Bool { false }
        public init() {}
        init(type: TokenType, text: String) { self.type = type; self.text = text }
    }

    open class RichExpression {
        open var tokens: [RichToken] = []
        open var isEmpty: Bool { tokens.isEmpty }
        open var isZero: Bool { tokens.count == 1 && tokens[0].text == "0" }
        public init() {}
    }

    public class Operation: Hashable {
        let name: String
        let text: String
        init(_ name: String, _ text: String) { self.name = name; self.text = text }
        public static func == (a: Operation, b: Operation) -> Bool { a.name == b.name }
        public func hash(into h: inout Hasher) { h.combine(name) }
        public static let leftRotate = ShiftOperation("leftRotate", "leftRotate")
        public static let rightShift = ShiftOperation("rightShift", "rightShift")
        public static let rightRotate = ShiftOperation("rightRotate", "rightRotate")
        public static let leftShiftCustom = ShiftOperation("leftShiftCustom", "leftShiftCustom")
        public static let rightShiftCustom = ShiftOperation("rightShiftCustom", "rightShiftCustom")
        public static let leftShift = ShiftOperation("leftShift", "leftShift")
        public static let reciprocal = ReciprocalOperation("reciprocal", "reciprocal")
        public static let squareRoot = RootOperation("squareRoot", "squareRoot")
        public static let reverseRoot = RootOperation("reverseRoot", "reverseRoot")
        public static let root = RootOperation("root", "root")
        public static let cubeRoot = RootOperation("cubeRoot", "cubeRoot")
        public static let eulerNumber = ConstantOperation("eulerNumber", "e")
        public static let pi = ConstantOperation("pi", "π")
        public static let inverseSine = FunctionOperation("inverseSine", "inverseSine")
        public static let inverseCosine = FunctionOperation("inverseCosine", "inverseCosine")
        public static let hyperbolicSine = FunctionOperation("hyperbolicSine", "hyperbolicSine")
        public static let inverseTangent = FunctionOperation("inverseTangent", "inverseTangent")
        public static let hyperbolicCosine = FunctionOperation("hyperbolicCosine", "hyperbolicCosine")
        public static let logarithmNatural = FunctionOperation("logarithmNatural", "logarithmNatural")
        public static let hyperbolicTangent = FunctionOperation("hyperbolicTangent", "hyperbolicTangent")
        public static let inverseHyperbolicSine = FunctionOperation("inverseHyperbolicSine", "inverseHyperbolicSine")
        public static let inverseHyperbolicCosine = FunctionOperation("inverseHyperbolicCosine", "inverseHyperbolicCosine")
        public static let inverseHyperbolicTangent = FunctionOperation("inverseHyperbolicTangent", "inverseHyperbolicTangent")
        public static let sine = FunctionOperation("sine", "sine")
        public static let cosine = FunctionOperation("cosine", "cosine")
        public static let tangent = FunctionOperation("tangent", "tangent")
        public static let logarithmBase2 = LogOperation("logarithmBase2", "logarithmBase2")
        public static let logarithmBase10 = LogOperation("logarithmBase10", "logarithmBase10")
        public static let logarithmBaseCustom = LogOperation("logarithmBaseCustom", "logarithmBaseCustom")
        public static let openParenthesis = OpenParenthesisOperation("openParenthesis", "(")
        public static let closeParenthesis = CloseParenthesisOperation("closeParenthesis", ")")
        public static let exponentialBase2 = ExponentialOperation("exponentialBase2", "exponentialBase2")
        public static let exponentialBase10 = ExponentialOperation("exponentialBase10", "exponentialBase10")
        public static let exponentialBaseEuler = ExponentialOperation("exponentialBaseEuler", "exponentialBaseEuler")
        public static let exponentialBaseCustom = ExponentialOperation("exponentialBaseCustom", "exponentialBaseCustom")
        public static let ee = LiteralOperation("ee", "E")
        public static let digit0 = LiteralOperation("digit0", "0")
        public static let digit1 = LiteralOperation("digit1", "1")
        public static let digit2 = LiteralOperation("digit2", "2")
        public static let digit3 = LiteralOperation("digit3", "3")
        public static let digit4 = LiteralOperation("digit4", "4")
        public static let digit5 = LiteralOperation("digit5", "5")
        public static let digit6 = LiteralOperation("digit6", "6")
        public static let digit7 = LiteralOperation("digit7", "7")
        public static let digit8 = LiteralOperation("digit8", "8")
        public static let digit9 = LiteralOperation("digit9", "9")
        public static let digitA = LiteralOperation("digitA", "A")
        public static let digitB = LiteralOperation("digitB", "B")
        public static let digitC = LiteralOperation("digitC", "C")
        public static let digitD = LiteralOperation("digitD", "D")
        public static let digitE = LiteralOperation("digitE", "E")
        public static let digitF = LiteralOperation("digitF", "F")
        public static let decimal = LiteralOperation("decimal", ".")
        public static let digit00 = LiteralOperation("digit00", "00")
        public static let digitFF = LiteralOperation("digitFF", "FF")
        public static let or = BasicOperatorOperation("or", "or")
        public static let add = BasicOperatorOperation("add", "+")
        public static let and = BasicOperatorOperation("and", "and")
        public static let mod = BasicOperatorOperation("mod", "mod")
        public static let nor = BasicOperatorOperation("nor", "nor")
        public static let xor = BasicOperatorOperation("xor", "xor")
        public static let divide = BasicOperatorOperation("divide", "÷")
        public static let percent = BasicOperatorOperation("percent", "%")
        public static let multiply = BasicOperatorOperation("multiply", "×")
        public static let subtract = BasicOperatorOperation("subtract", "-")
        public static let neg = NegOperation("neg", "neg")
        public static let not = NotOperation("not", "not")
        public static let cube = PowerOperation("cube", "cube")
        public static let power = PowerOperation("power", "power")
        public static let square = PowerOperation("square", "square")
        public static let drop = DropOperation("drop", "drop")
        public static let swap = SwapOperation("swap", "swap")
        public static let clear = ClearOperation("clear", "clear")
        public static let enter = EnterOperation("enter", "enter")
        public static let delete = DeleteOperation("delete", "delete")
        public static let equals = EqualsOperation("equals", "equals")
        public static let negate = NegateOperation("negate", "negate")
        public static let random = RandOperation("random", "random")
        public static let rollIn = RollInOperation("rollIn", "rollIn")
        public static let rollOut = RollOutOperation("rollOut", "rollOut")
        public static let allClear = AllClearOperation("allClear", "allClear")
        public static let byteFlip = FlipOperation("byteFlip", "byteFlip")
        public static let wordFlip = FlipOperation("wordFlip", "wordFlip")
        public static let factorial = FactorialOperation("factorial", "factorial")
    }

    public class ShiftOperation: Operation {}
    public class ReciprocalOperation: Operation {}
    public class RootOperation: Operation {}
    public class ConstantOperation: Operation {}
    public class FunctionOperation: Operation {}
    public class LogOperation: Operation {}
    public class OpenParenthesisOperation: Operation {}
    public class CloseParenthesisOperation: Operation {}
    public class ExponentialOperation: Operation {}
    public class LiteralOperation: Operation {}
    public class BasicOperatorOperation: Operation {}
    public class NegOperation: Operation {}
    public class NotOperation: Operation {}
    public class PowerOperation: Operation {}
    public class DropOperation: Operation {}
    public class SwapOperation: Operation {}
    public class ClearOperation: Operation {}
    public class EnterOperation: Operation {}
    public class DeleteOperation: Operation {}
    public class EqualsOperation: Operation {}
    public class NegateOperation: Operation {}
    public class RandOperation: Operation {}
    public class RollInOperation: Operation {}
    public class RollOutOperation: Operation {}
    public class AllClearOperation: Operation {}
    public class FlipOperation: Operation {}
    public class FactorialOperation: Operation {}
    public final class CustomOperandOperation: Operation {
        let result: CalculateResult
        public init(_ result: CalculateResult) { self.result = result; super.init("custom", result.formattedValue) }
    }
}

