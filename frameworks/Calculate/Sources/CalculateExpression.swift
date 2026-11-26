// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
@_exported import CalculateObjC

// Minimal engine of milestone 1: types and API surface are Apple's, the arithmetic is a plain
// left-to-right calculator on decimal numbers. Faithful behaviour is milestone 2.
public final class CalculateExpression: Hashable, CustomDebugStringConvertible {
    let uuid: UUID
    public var options: [CalculateKey: Any]
    public var base: Base
    public var engine: CalculateEngine = CalculateEngine(rawValue: 1)
    public var format: CalculateFormat = CalculateFormat(rawValue: 0)
    public var locales: [Locale] = [Locale.current]
    public var numberFormatter: NumberFormatter?
    public var allowsArabicMath: Bool = false
    public var assumeDegrees: Bool = true
    public var bits: [Bool] = Array(repeating: false, count: 64)
    public private(set) var result: CalculateResult?
    public private(set) var error: Error?
    public private(set) var activeOperation: Operation?
    public private(set) var postfixStack: [CalculateExpression] = []
    public private(set) var removed: RichExpression?
    public private(set) var activeError: Error?
    public var expression: String
    var accumulator: Double = 0
    var pending: Operation?
    var entering = true
    var repeatable: (Operation, Double)?

    public init(_ expression: String, options: [CalculateKey: Any], base: Base, id: UUID) {
        self.expression = expression
        self.options = options
        self.base = base
        self.uuid = id
    }

    public convenience init(options: [CalculateKey: Any], base: Base, id: UUID) {
        self.init("0", options: options, base: base, id: id)
    }

    public convenience init(result: CalculateResult?, error: Error?, options: [CalculateKey: Any], base: Base, id: UUID) {
        self.init(result?.formattedResult ?? "0", options: options, base: base, id: id)
        self.result = result
        self.error = error
    }

    public static func == (a: CalculateExpression, b: CalculateExpression) -> Bool { a.uuid == b.uuid }
    public func hash(into h: inout Hasher) { h.combine(uuid) }

    public var debugDescription: String { "CalculateExpression(\(expression))" }
    public var copyableExpression: String { expression }
    public var unicode: String { expression }
    public var ascii: String {
        expression.replacingOccurrences(of: "×", with: "*").replacingOccurrences(of: "÷", with: "/")
    }
    public var prefersRTL: Bool { false }
    public var prefersClear: Bool { entering && expression != "0" }
    public var allowedOperations: Set<Operation> { [] }
    public var accessibilityMathEquation: [AnyHashable: Any] { [:] }
    public var accessibilityMathEquationDescription: AttributedString { AttributedString(expression) }

    public var plain: PlainExpression {
        let p = PlainExpression()
        p.tokens = [Token(type: .other, text: expression)]
        return p
    }

    public var rich: RichExpression {
        let r = RichExpression()
        r.tokens = [RichToken(type: .other, text: expression)]
        return r
    }

    public func clearAll() {
        expression = "0"; accumulator = 0; pending = nil; entering = true; result = nil; error = nil; activeOperation = nil
    }

    public func repeatOperation() {
        guard let (op, operand) = repeatable else { return }
        pending = op
        expression = Self.format(operand)
        entering = true
        _ = update(.equals)
    }

    public func setRepeatableOperation(from other: CalculateExpression) { repeatable = other.repeatable }

    public func inlineEvaluate() {
        _ = update(.equals)
    }

    public func inlineEvaluate(_ other: CalculateExpression) {
        expression = other.expression
        accumulator = other.accumulator
    }

    static func format(_ v: Double) -> String {
        if v.isNaN || v.isInfinite { return "Error" }
        if v == v.rounded() && abs(v) < 1e15 { return String(Int64(v)) }
        var s = String(format: "%.9f", v)
        while s.hasSuffix("0") { s.removeLast() }
        return s
    }

    func apply(_ op: Operation, _ a: Double, _ b: Double) -> Double {
        switch op.name {
        case "add": return a + b
        case "subtract": return a - b
        case "multiply": return a * b
        case "divide": return a / b
        case "percent": return a.truncatingRemainder(dividingBy: b)
        default: return b
        }
    }

    @discardableResult
    public func update(_ op: Operation) -> Bool {
        let n = op.name
        if n.hasPrefix("digit") || n == "decimal" {
            if entering && expression == "0" && n != "decimal" { expression = "" }
            if !entering { expression = "0"; entering = true; if n != "decimal" { expression = "" } }
            if n == "decimal" && expression.contains(".") { return false }
            expression += op.text
            if expression.isEmpty { expression = "0" }
            return true
        }
        switch n {
        case "allClear", "clear":
            clearAll(); return true
        case "delete":
            expression.removeLast(); if expression.isEmpty { expression = "0" }; return true
        case "negate", "neg":
            expression = expression.hasPrefix("-") ? String(expression.dropFirst()) : "-" + expression; return true
        case "square", "cube", "reciprocal", "squareRoot", "factorial", "pi":
            let v = Double(expression) ?? 0
            var r = v
            switch n {
            case "square": r = v * v
            case "cube": r = v * v * v
            case "reciprocal": r = 1 / v
            case "squareRoot": r = v.squareRoot()
            case "factorial": r = tgamma(v + 1)
            default: r = Double.pi
            }
            expression = Self.format(r); entering = false; return true
        case "equals":
            if let p = pending {
                let b = Double(expression) ?? 0
                accumulator = apply(p, accumulator, b)
                repeatable = (p, b)
                expression = Self.format(accumulator)
                result = CalculateResult(value: accumulator)
                pending = nil
            }
            entering = false
            return true
        case "add", "subtract", "multiply", "divide", "percent":
            if let p = pending, entering {
                accumulator = apply(p, accumulator, Double(expression) ?? 0)
            } else {
                accumulator = Double(expression) ?? 0
            }
            expression = Self.format(accumulator)
            pending = op; entering = false; activeOperation = op
            return true
        default:
            return false
        }
    }
}

public func evaluate(_ string: String, options: [CalculateKey: Any]) -> CalculateResult? {
    let e = NSExpression(format: string.replacingOccurrences(of: "×", with: "*").replacingOccurrences(of: "÷", with: "/"))
    guard let v = e.expressionValue(with: nil, context: nil) as? NSNumber else { return nil }
    return CalculateResult(value: v.doubleValue)
}

public func evaluateError(_ string: String, options: [CalculateKey: Any]) throws -> CalculateResult {
    if let r = evaluate(string, options: options) { return r }
    throw NSError(domain: "CalculateErrorDomain", code: 1)
}

open class StocksKitCurrencyCache {
    public static let shared = StocksKitCurrencyCache()
    public static var isEnabled: Bool { false }
    open var needsRefresh: Bool { false }
    open var lastRefreshDate: Date? { get async { nil } }
    open func refresh() async -> Bool { false }
    init() {}
}

extension CalculateUnit: @retroactive Identifiable {
    public var id: String { name }
}

extension CalculateUnitCategory: @retroactive Identifiable {
    public var id: String { name }
}

extension CalculateResult {
    public func converted(to unit: CalculateUnit, from other: CalculateUnit) -> CalculateResult? {
        converted(to: unit)
    }
}
