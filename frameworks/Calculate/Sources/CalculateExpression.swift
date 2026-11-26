// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
@_exported import CalculateObjC

// Minimal engine of milestone 1: types and API surface are Apple's, the arithmetic is a plain
// left-to-right calculator on decimal numbers. Faithful behaviour is milestone 2.
// MARK: tokens

enum Tok: Equatable {
    case num(String)      // typed digits (with "." when a decimal point is pending) or an evaluated "123E-2"
    case op(String)       // + - × ÷ ^
    case pct, fact
    case open, close
    case neg              // unary minus of a negated operand: ( - x )
    case fn(String)       // sin cos tan ln log ...: followed by .open
    case sqrt
    case const(String)    // π 𝑒
    case ee(String)       // exponent digits of 2e3

    var isOperand: Bool {
        switch self {
        case .num, .close, .const, .pct, .fact, .ee: return true
        default: return false
        }
    }
}

struct EngineError: Error { let code: Int; let reason: String }

// Minimal engine of milestone 2: an infix expression editor with live evaluation in decimal arithmetic.
// The rules come from observing Apple's Calculate.framework (see EVIDENCE.md).
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
    public private(set) var postfixStack: [CalculateExpression] = []
    public private(set) var removed: RichExpression?

    var tokens: [Tok] = []
    var openDepth = 0
    var entering = false
    var clearsEntry = true
    var repeatable: (String, [Tok])?
    var pendingExponent = false

    public init(_ expression: String, options: [CalculateKey: Any], base: Base, id: UUID) {
        self.options = options
        self.base = base
        self.uuid = id
        if let v = options[.assumeDegrees] as? Bool { assumeDegrees = v }
        if expression != "0", !expression.isEmpty, let n = Decimal(string: expression.replacingOccurrences(of: ",", with: "")) { tokens = [.num(Self.raw(n))] }
    }

    public convenience init(options: [CalculateKey: Any], base: Base, id: UUID) {
        self.init("0", options: options, base: base, id: id)
    }

    public convenience init(result: CalculateResult?, error: Error?, options: [CalculateKey: Any], base: Base, id: UUID) {
        self.init("0", options: options, base: base, id: id)
        if let r = result { tokens = [.num(Self.raw(r.decimal as Decimal))] }
    }

    public static func == (a: CalculateExpression, b: CalculateExpression) -> Bool { a.uuid == b.uuid }
    public func hash(into h: inout Hasher) { h.combine(uuid) }
    public var debugDescription: String { "CalculateExpression(\(expression))" }

    // MARK: display

    static func group(_ digits: String) -> String {
        var out = ""
        for (i, c) in digits.reversed().enumerated() {
            if i > 0 && i % 3 == 0 { out.append(",") }
            out.append(c)
        }
        return String(out.reversed())
    }

    func text(_ t: Tok, raw: Bool = false) -> String {
        switch t {
        case .num(let s):
            if s.contains("E") { return s }
            if raw { return s }
            var s = s
            if s.hasSuffix(".") { s.removeLast() }
            let parts = s.split(separator: ".", omittingEmptySubsequences: false)
            let neg = parts[0].hasPrefix("-")
            let ip = neg ? String(parts[0].dropFirst()) : String(parts[0])
            return (neg ? "-" : "") + Self.group(ip) + (parts.count > 1 ? "." + parts[1] : "")
        case .op(let s): return raw && s == "^" ? "" : s
        case .pct: return "%"
        case .fact: return "!"
        case .open: return "("
        case .close: return ")"
        case .neg: return "-"
        case .fn(let s): return s
        case .sqrt: return "√"
        case .const(let s): return raw && s == "𝑒" ? "e" : s
        case .ee(let s): return "e" + s
        }
    }

    public var expression: String {
        get {
            if tokens.isEmpty { return "0" }
            return tokens.map { text($0) }.joined() + String(repeating: ")", count: openDepth)
        }
        set {
            tokens = []; openDepth = 0
            let plain = newValue.replacingOccurrences(of: ",", with: "")
            if plain != "0" && plain != "", Decimal(string: plain) != nil { tokens = [.num(plain)] }
        }
    }

    public var copyableExpression: String { expression }
    public var unicode: String { expression }
    public var ascii: String { expression.replacingOccurrences(of: "×", with: "*").replacingOccurrences(of: "÷", with: "/") }
    public var prefersRTL: Bool { false }
    public var prefersClear: Bool { !tokens.isEmpty && clearsEntry }
    public var allowedOperations: Set<Operation> { [] }
    public var accessibilityMathEquation: [AnyHashable: Any] { [:] }
    public var accessibilityMathEquationDescription: AttributedString { AttributedString(expression) }
    public var activeOperation: Operation? { nil }
    public var activeError: Error? { error }

    func tokenType(_ t: Tok) -> TokenType {
        switch t {
        case .op(let s):
            switch s { case "+": return .add; case "-": return .subtract; case "×": return .multiply; case "÷": return .divide; default: return .exponentiate }
        case .pct: return .percent
        case .fact: return .factorial
        case .open: return .openParenthesis
        case .close: return .closeParenthesis
        case .neg: return .neg
        case .fn, .sqrt: return .function
        case .const(let s): return s == "π" ? .pi : .eulerNumber
        case .num, .ee: return .number
        }
    }

    public var plain: PlainExpression {
        let p = PlainExpression()
        p.tokens = tokens.map { Token(type: tokenType($0), text: text($0)) }
        return p
    }

    public var rich: RichExpression {
        var toks = tokens + Array(repeating: Tok.close, count: openDepth)
        if toks.isEmpty { toks = [.num("0")] }
        return richExpression(toks)
    }

    // The exponent operand of ^ is the "right" expression of the ^ token; a square root keeps its operand in parentheses.
    func richExpression(_ toks: [Tok]) -> RichExpression {
        let r = RichExpression()
        var i = 0
        while i < toks.count {
            let t = toks[i]
            if case .op("^") = t {
                var j = i + 1
                var end = j
                if j < toks.count {
                    switch toks[j] {
                    case .open:
                        var d = 0
                        while end < toks.count { if toks[end] == .open { d += 1 }; if toks[end] == .close { d -= 1; if d == 0 { end += 1; break } }; end += 1 }
                    case .fn:
                        var d = 0; end = j + 1
                        while end < toks.count { if toks[end] == .open { d += 1 }; if toks[end] == .close { d -= 1; if d == 0 { end += 1; break } }; end += 1 }
                    default: end = j + 1
                    }
                    if end < toks.count, case .ee = toks[end] { end += 1 }
                    if end < toks.count, case .op("^") = toks[end] {
                        // right associative: the next ^ belongs to the exponent
                        var k = end + 1
                        if k < toks.count { k += 1 }
                        end = k
                    }
                }
                j = min(j, toks.count)
                let right = richExpression(Array(toks[j..<min(end, toks.count)]))
                r.tokens.append(ExponentToken(right: right))
                i = min(end, toks.count)
                continue
            }
            if case .sqrt = t {
                r.tokens.append(RichToken(type: .function, text: "√"))
                // operand in parentheses
                var j = i + 1
                var end = j
                if j < toks.count {
                    if toks[j] == .open { var d = 0; while end < toks.count { if toks[end] == .open { d += 1 }; if toks[end] == .close { d -= 1; if d == 0 { end += 1; break } }; end += 1 } }
                    else { end = j + 1 }
                }
                if toks[j..<min(end, toks.count)].first == .open {
                    r.tokens += richExpression(Array(toks[j..<min(end, toks.count)])).tokens
                } else {
                    r.tokens.append(RichToken(type: .openParenthesis, text: "("))
                    r.tokens += richExpression(Array(toks[j..<min(end, toks.count)])).tokens
                    r.tokens.append(RichToken(type: .closeParenthesis, text: ")"))
                }
                i = min(end, toks.count)
                continue
            }
            r.tokens.append(RichToken(type: tokenType(t), text: text(t, raw: true)))
            i += 1
        }
        return r
    }

    // MARK: evaluation

    var evaluation: Result<Decimal, EngineError> {
        if tokens.isEmpty { return .success(0) }
        var toks = tokens
        toks += Array(repeating: .close, count: openDepth)
        var p = Parser(tokens: toks, degrees: assumeDegrees)
        do {
            let v = try p.parseAll()
            return .success(v)
        } catch let e as EngineError { return .failure(e) } catch { return .failure(EngineError(code: -1000, reason: "Math error")) }
    }

    public var result: CalculateResult? {
        if case .success(let v) = evaluation { return CalculateResult(decimal: v as NSDecimalNumber, formatter: numberFormatter) }
        return nil
    }

    public var error: Error? {
        if case .failure(let e) = evaluation {
            let info = e.code == -1000
                ? [NSLocalizedDescriptionKey: "Math error", NSLocalizedFailureReasonErrorKey: "Check the expression for math errors and spelling mistakes"]
                : [NSLocalizedDescriptionKey: e.reason, NSLocalizedFailureReasonErrorKey: e.code == -2004 ? "Numbers cannot be divided by zero" : e.reason]
            return NSError(domain: "com.apple.calculateframework", code: e.code, userInfo: info)
        }
        return nil
    }

    static func magnitude(_ d: Decimal) -> Int {
        let x = abs(NSDecimalNumber(decimal: d).doubleValue)
        guard x.isFinite, x > 0 else { return 0 }
        return Int(floor(log10(x)))
    }

    static func raw(_ d: Decimal) -> String {
        var v = d
        var r = Decimal()
        NSDecimalRound(&r, &v, 33 - magnitude(d), .plain)
        let n = NSDecimalNumber(decimal: r)
        var mant = r.significand
        var s = NSDecimalNumber(decimal: mant).stringValue
        mant = 0
        let neg = (r < 0)
        if neg { s = "-" + s.replacingOccurrences(of: "-", with: "") }
        _ = n
        let e = r.exponent
        return s + "E" + (e < 0 ? "-" : "+") + String(abs(e))
    }

    public func clearAll() {
        tokens = []; openDepth = 0; entering = false; clearsEntry = false; repeatable = nil; pendingExponent = false
    }

    public func repeatOperation() {
        guard let (op, operand) = repeatable else { return }
        tokens += [.op(op)] + operand
        repeatable = nil
        entering = false
    }

    public func setRepeatableOperation(from other: CalculateExpression) { repeatable = other.repeatable }

    public func inlineEvaluate(_ other: CalculateExpression) {
        tokens = other.tokens; openDepth = other.openDepth; entering = other.entering
    }

    public func inlineEvaluate() {
        switch evaluation {
        case .success(let v):
            // the operation "=" repeats: the last top-level binary operation with its right operand
            repeatable = lastBinary()
            tokens = [.num(Self.raw(v))]
        case .failure:
            tokens = []
        }
        openDepth = 0; entering = false; clearsEntry = false
    }

    func lastBinary() -> (String, [Tok])? {
        var depth = openDepth > 0 ? 0 : 0
        var idx: Int?
        for (i, t) in tokens.enumerated() {
            switch t {
            case .open: depth += 1
            case .close: depth -= 1
            case .op(let s) where depth == 0 && ["+", "-", "×", "÷"].contains(s) && i > 0: idx = i
            default: break
            }
        }
        guard openDepth == 0, let i = idx, i + 1 < tokens.count, case .op(let s) = tokens[i] else { return nil }
        return (s, Array(tokens[(i + 1)...]))
    }

    // MARK: editing

    /// Range of the last operand: a number, a constant, a closed group (with its function or sign) and trailing % or !.
    func lastOperand() -> Range<Int>? {
        var end = tokens.count
        guard end > 0 else { return nil }
        while end > 0, case .pct = tokens[end - 1] { end -= 1 }
        // a percent applied to a group keeps the group
        var j = tokens.count
        while j > 0, tokens[j - 1] == .pct || tokens[j - 1] == .fact { j -= 1 }
        guard j > 0 else { return nil }
        let last = tokens[j - 1]
        var start = j - 1
        switch last {
        case .num, .const:
            if case .ee = last { }
        case .ee:
            start = j - 2
            if start >= 0, case .num = tokens[start] { } else { return nil }
        case .close:
            var depth = 0
            var i = j - 1
            while i >= 0 {
                if tokens[i] == .close { depth += 1 }
                if tokens[i] == .open { depth -= 1; if depth == 0 { break } }
                i -= 1
            }
            guard i >= 0 else { return nil }
            start = i
            if i > 0, case .fn = tokens[i - 1] { start = i - 1 }
        default:
            return nil
        }
        if start > 0, tokens[start - 1] == .sqrt { start -= 1 }
        return start..<tokens.count
    }

    func append(operand t: Tok) {
        if let last = tokens.last, last.isOperand, !(last == .pct) { tokens.append(.op("×")) }
        tokens.append(t)
    }

    @discardableResult
    public func update(_ op: Operation) -> Bool {
        let n = op.name
        clearsEntry = true
        if n.hasPrefix("digit") && n != "digit00" && n != "digitFF" && n.count == 6 {
            return typeDigit(String(n.last!))
        }
        switch n {
        case "digit00": return typeDigit("0") && typeDigit("0")
        case "decimal": return typeDecimal()
        case "add": return binary("+")
        case "subtract": return binary("-")
        case "multiply": return binary("×")
        case "divide": return binary("÷")
        case "power": return binary("^")
        case "equals": return true
        case "allClear": clearAll(); return true
        case "clear": return clearEntry()
        case "delete": return deleteLast()
        case "negate": return negate()
        case "reciprocal":
            guard let r = lastOperand() else { return false }
            if tokens[r] == [.num("1")] { return false }
            tokens.replaceSubrange(r, with: [.open, .num("1"), .op("÷")] + tokens[r] + [.close]); entering = false; return true
        case "square": return power(2)
        case "cube": return power(3)
        case "squareRoot":
            guard let r = lastOperand() else { return false }
            tokens.insert(.sqrt, at: r.lowerBound); entering = false; return true
        case "factorial":
            guard lastOperand() != nil else { return false }
            tokens.append(.fact); entering = false; return true
        case "percent":
            guard let r = lastOperand() else { return false }
            if tokens.last == .pct { tokens.insert(.open, at: r.lowerBound); tokens += [.close, .pct] } else { tokens.append(.pct) }
            entering = false; return true
        case "pi": return constant("π")
        case "eulerNumber": return constant("𝑒")
        case "ee": pendingExponent = lastOperand() != nil; return pendingExponent
        case "openParenthesis":
            if tokens.last?.isOperand == true { tokens.append(.op("×")) }
            tokens.append(.open); openDepth += 1; entering = false; return true
        case "closeParenthesis":
            guard openDepth > 0 else { return false }
            tokens.append(.close); openDepth -= 1; entering = false; return true
        case "sine": return function("sin")
        case "cosine": return function("cos")
        case "tangent": return function("tan")
        case "logarithmNatural": return function("ln")
        case "logarithmBase10": return function("log")
        case "inverseSine": return function("sin⁻¹")
        case "inverseCosine": return function("cos⁻¹")
        case "inverseTangent": return function("tan⁻¹")
        case "hyperbolicSine": return function("sinh")
        case "hyperbolicCosine": return function("cosh")
        case "hyperbolicTangent": return function("tanh")
        default: return false
        }
    }

    func typeDigit(_ d: String) -> Bool {
        if pendingExponent {
            pendingExponent = false
            tokens.append(.ee(d)); entering = true; return true
        }
        if case .ee(let s)? = tokens.last { tokens[tokens.count - 1] = .ee(s + d); return true }
        if entering, case .num(let s)? = tokens.last {
            tokens[tokens.count - 1] = .num(s == "0" ? d : s + d)
            return true
        }
        if tokens.last?.isOperand == true, tokens.last != .pct { tokens.append(.op("×")) }
        tokens.append(.num(d)); entering = true
        return true
    }

    func typeDecimal() -> Bool {
        if entering, case .num(let s)? = tokens.last {
            if s.contains(".") { return false }
            tokens[tokens.count - 1] = .num(s + ".")
            return true
        }
        if tokens.last?.isOperand == true, tokens.last != .pct { tokens.append(.op("×")) }
        tokens.append(.num("0.")); entering = true
        return true
    }

    func binary(_ s: String) -> Bool {
        if tokens.isEmpty { tokens = [.num("0")] }
        switch tokens.last! {
        case .op: tokens[tokens.count - 1] = .op(s)
        case .open, .fn, .sqrt, .neg: return false
        default: tokens.append(.op(s))
        }
        entering = false
        return true
    }

    func power(_ p: Int) -> Bool {
        guard lastOperand() != nil else { return false }
        tokens += [.op("^"), .num(String(p))]
        entering = false
        return true
    }

    func constant(_ c: String) -> Bool {
        if case .const? = tokens.last { tokens.append(.op("×")) }
        else if let l = tokens.last, l.isOperand, l != .num("") {
            if case .num = l { } else { tokens.append(.op("×")) }
        }
        tokens.append(.const(c)); entering = false; return true
    }

    func function(_ name: String) -> Bool {
        if let r = lastOperand(), !tokens.isEmpty, tokens.last?.isOperand == true {
            tokens.replaceSubrange(r, with: [.fn(name), .open] + tokens[r] + [.close])
        } else {
            tokens += [.fn(name), .open]; openDepth += 1
        }
        entering = false
        return true
    }

    func negate() -> Bool {
        guard let r = lastOperand(), tokens.last?.isOperand == true else { return false }
        let slice = Array(tokens[r])
        if slice.count >= 3, slice.first == .open, slice[1] == .neg, slice.last == .close {
            tokens.replaceSubrange(r, with: slice[2..<(slice.count - 1)])
        } else {
            tokens.replaceSubrange(r, with: [.open, .neg] + slice + [.close])
        }
        entering = false
        return true
    }

    func clearEntry() -> Bool {
        if let r = lastOperand(), tokens.last?.isOperand == true {
            tokens.removeSubrange(r)
            entering = false
        }
        clearsEntry = false
        return true
    }

    func deleteLast() -> Bool {
        guard let last = tokens.last else { return true }
        switch last {
        case .num(let s):
            if s.count > 1 && !s.contains("E") {
                let t = String(s.dropLast())
                tokens[tokens.count - 1] = .num(t == "-" ? "0" : t)
            } else { tokens.removeLast() }
        case .ee(let s):
            if s.count > 1 { tokens[tokens.count - 1] = .ee(String(s.dropLast())) } else { tokens.removeLast() }
        case .open:
            tokens.removeLast(); openDepth -= 1
            if case .fn? = tokens.last { tokens.removeLast() }
        case .close:
            tokens.removeLast(); openDepth += 1
        default: tokens.removeLast()
        }
        if tokens.isEmpty { clearsEntry = false }
        entering = { if case .num? = tokens.last { return true } else { return false } }()
        return true
    }
}

final class ExponentToken: CalculateExpression.RichToken {
    let exponent: CalculateExpression.RichExpression
    init(right: CalculateExpression.RichExpression) { exponent = right; super.init(type: .exponentiate, text: "") }
    override var right: CalculateExpression.RichExpression? { exponent }
}

// MARK: parser

struct Parser {
    var tokens: [Tok]
    var degrees: Bool
    var i = 0

    init(tokens: [Tok], degrees: Bool) { self.tokens = tokens; self.degrees = degrees }

    func math() -> EngineError { EngineError(code: -1000, reason: "Math error") }

    mutating func parseAll() throws -> Decimal {
        let v = try additive()
        guard i == tokens.count else { throw math() }
        return Parser.round(v)
    }

    static func round(_ d: Decimal) -> Decimal {
        var v = d, r = Decimal()
        if d.isNaN { return d }
        NSDecimalRound(&r, &v, max(-60, 33 - CalculateExpression.magnitude(d)), .plain)
        return r
    }

    func peek() -> Tok? { i < tokens.count ? tokens[i] : nil }

    mutating func additive() throws -> Decimal {
        var left = try term()
        while case .op(let s)? = peek(), s == "+" || s == "-" {
            i += 1
            let (right, isPercent) = try termPercent()
            let amount = isPercent ? left * right : right
            left = s == "+" ? left + amount : left - amount
        }
        return left
    }

    // the right operand of + or -: a plain percentage is a share of the left operand
    mutating func termPercent() throws -> (Decimal, Bool) {
        let start = i
        let v = try term()
        // a term that is nothing but x% (possibly repeated)
        var j = i - 1
        guard j >= start, tokens[j] == .pct else { return (v, false) }
        while j >= start, tokens[j] == .pct { j -= 1 }
        // everything before the percent signs is a single operand
        var p = Parser(tokens: Array(tokens[start...j]), degrees: degrees)
        if (try? p.primaryOnly()) != nil, p.i == p.tokens.count { return (v, true) }
        return (v, false)
    }

    mutating func primaryOnly() throws -> Decimal { try postfixBase() }

    mutating func term() throws -> Decimal {
        var left = try power()
        while true {
            if case .op(let s)? = peek(), s == "×" || s == "÷" {
                i += 1
                let right = try power()
                if s == "×" { left *= right }
                else {
                    if right == 0 { throw EngineError(code: -2004, reason: "Undefined") }
                    left = Parser.divide(left, right)
                }
            } else if case .pct? = peek(), i + 1 < tokens.count, startsOperand(tokens[i + 1]) {
                i += 1   // binary modulo: 5%2
                let right = try power()
                if right == 0 { throw EngineError(code: -2004, reason: "Undefined") }
                left = Parser.mod(left, right)
            } else if let t = peek(), startsOperand(t), !(t == .close) {
                // implicit multiplication: 2π, 3(4)
                if case .num = t { break }
                let right = try power()
                left *= right
            } else { break }
        }
        return left
    }

    func startsOperand(_ t: Tok) -> Bool {
        switch t {
        case .num, .open, .fn, .sqrt, .const, .neg, .ee: return true
        default: return false
        }
    }

    static func divide(_ a: Decimal, _ b: Decimal) -> Decimal { a / b }
    static func mod(_ a: Decimal, _ b: Decimal) -> Decimal {
        let q = NSDecimalNumber(decimal: a / b).rounding(accordingToBehavior: NSDecimalNumberHandler(roundingMode: .down, scale: 0, raiseOnExactness: false, raiseOnOverflow: false, raiseOnUnderflow: false, raiseOnDivideByZero: false)).decimalValue
        return a - q * b
    }

    mutating func power() throws -> Decimal {
        let base = try postfix()
        if case .op("^")? = peek() {
            i += 1
            let exp = try power()
            return try Parser.pow(base, exp)
        }
        return base
    }

    static func pow(_ a: Decimal, _ b: Decimal) throws -> Decimal {
        let bd = NSDecimalNumber(decimal: b).doubleValue
        if bd == bd.rounded(), abs(bd) < 10000 {
            let n = Int(bd)
            if Double(n) * log10(max(abs(NSDecimalNumber(decimal: a).doubleValue), 1e-300)) > 120 { throw EngineError(code: -2004, reason: "Undefined") }
            if n >= 0 { return Foundation.pow(a, n) }
            if a == 0 { throw EngineError(code: -2004, reason: "Undefined") }
            return 1 / Foundation.pow(a, -n)
        }
        let r = Foundation.pow(NSDecimalNumber(decimal: a).doubleValue, bd)
        guard r.isFinite else { throw EngineError(code: -2004, reason: "Undefined") }
        return Decimal(r)
    }

    mutating func postfix() throws -> Decimal {
        var v = try postfixBase()
        while true {
            if case .pct? = peek(), !(i + 1 < tokens.count && startsOperand(tokens[i + 1])) { i += 1; v /= 100 }
            else if case .fact? = peek() { i += 1; v = try Parser.factorial(v) }
            else { break }
        }
        return v
    }

    static func factorial(_ v: Decimal) throws -> Decimal {
        let d = NSDecimalNumber(decimal: v).doubleValue
        if d < 0 { throw EngineError(code: -2004, reason: "Undefined") }
        if d == d.rounded() && d <= 170 {
            var r = Decimal(1)
            if d >= 2 { for k in 2...Int(d) { r *= Decimal(k) } }
            return r
        }
        let g = tgamma(d + 1)
        guard g.isFinite else { throw EngineError(code: -2004, reason: "Undefined") }
        return Decimal(g)
    }

    mutating func postfixBase() throws -> Decimal {
        guard let t = peek() else { throw math() }
        switch t {
        case .num(let s):
            i += 1
            var v: Decimal
            if s.contains("E") {
                let parts = s.split(separator: "E")
                v = Decimal(string: String(parts[0])) ?? 0
                let e = Int(parts[1].replacingOccurrences(of: "+", with: "")) ?? 0
                v = v * Foundation.pow(10, e >= 0 ? e : 0) / (e < 0 ? Foundation.pow(10, -e) : 1)
            } else {
                v = Decimal(string: s.hasSuffix(".") ? String(s.dropLast()) : s) ?? 0
            }
            if case .ee(let ex)? = peek() {
                i += 1
                v *= Foundation.pow(10, Int(ex) ?? 0)
            }
            return v
        case .const(let c):
            i += 1
            return c == "π" ? Decimal(Double.pi) : Decimal(M_E)
        case .neg:
            i += 1
            return -(try power())
        case .sqrt:
            i += 1
            let v = try postfixBase()
            if v < 0 { throw EngineError(code: -2004, reason: "Undefined") }
            return Decimal(NSDecimalNumber(decimal: v).doubleValue.squareRoot())
        case .open:
            i += 1
            let v = try additive()
            guard case .close? = peek() else { throw math() }
            i += 1
            return v
        case .fn(let name):
            i += 1
            guard case .open? = peek() else { throw math() }
            i += 1
            let v = try additive()
            guard case .close? = peek() else { throw math() }
            i += 1
            return try apply(name, v)
        default:
            throw math()
        }
    }

    func apply(_ name: String, _ v: Decimal) throws -> Decimal {
        let x = NSDecimalNumber(decimal: v).doubleValue
        let rad = degrees ? x * .pi / 180 : x
        func fix(_ r: Double) -> Decimal {
            // sin(180°) is 0, not 1.2e-16
            let rr = (abs(r) < 1e-15) ? 0 : (abs(r - r.rounded()) < 1e-15 ? r.rounded() : r)
            return Decimal(rr)
        }
        switch name {
        case "sin": return fix(Foundation.sin(rad))
        case "cos": return fix(Foundation.cos(rad))
        case "tan":
            let c = Foundation.cos(rad)
            if abs(c) < 1e-15 { throw EngineError(code: -2004, reason: "Undefined") }
            return fix(Foundation.tan(rad))
        case "ln":
            if x <= 0 { throw EngineError(code: -2004, reason: "Undefined") }
            return fix(Foundation.log(x))
        case "log":
            if x <= 0 { throw EngineError(code: -2004, reason: "Undefined") }
            return fix(Foundation.log10(x))
        case "sinh": return fix(Foundation.sinh(x))
        case "cosh": return fix(Foundation.cosh(x))
        case "tanh": return fix(Foundation.tanh(x))
        case "sin⁻¹": return fix(degrees ? asin(x) * 180 / .pi : asin(x))
        case "cos⁻¹": return fix(degrees ? acos(x) * 180 / .pi : acos(x))
        case "tan⁻¹": return fix(degrees ? atan(x) * 180 / .pi : atan(x))
        default: throw math()
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
