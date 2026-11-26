# Calculate: what the engine copies and how it was observed

All observations come from Apple's real Calculate.framework (macOS 26.7.2, Mac VM). The test program
`tests/main.swift` is built against our Calculate module and linked to `out/lib/Calculate.dylib`, whose install
name is the framework's; on the Mac dyld resolves that path to the real framework in the shared cache, on Reef to
ours. `tests/compare.sh` runs `tests/sequences.txt` on both and diffs the output (state after every key).
State of 110 sequences: identical except results beyond the range of Foundation's `Decimal`
(2^1000 and factorials from 99!; Apple gives 1.0715×10³⁰¹, 9.427×10¹⁵³ ...).

Observed model (all through `CalculateExpression.update(_:)`, `inlineEvaluate()`, `repeatOperation()`):
- `expression` is the formula string with grouping separators ("1,234", "12+3×4", "(-5)", "√9", "5^2", "2π",
  "10𝑒", "sin(30)", "sin^-1(1)"); the result is live: `result` is nil while the formula is incomplete ("5+").
  The error is then -1000 "Math error". Division by zero: -2004 "Undefined" ("Numbers cannot be divided by zero");
  domain errors (√ of a negative, ln of a negative, asin(2), tan(90°), 0^0, (-1)!): -2003 "Undefined"
  ("Input value is out of range"); ln(0) is -2004; overflow: -2000 "Overflow" ("Result is too large").
- `update(.equals)` changes nothing. `inlineEvaluate()` replaces the expression by the result in the form
  mantissa "E" exponent with 34 significant digits ("8E+0", "3333333333333333333333333333333333E-34", "10E+0");
  after a division by zero the expression is "0". `repeatOperation()` appends the last top-level binary operation
  and its right operand of the evaluated formula ("8E+0+3") once; it is armed by `inlineEvaluate()`.
- Precedence: ^ (right associative, 2^3^2 = 512) above × ÷ above + -; implicit multiplication (2π, 3(4)); a digit
  after a constant, ")" or an evaluated result starts a product ("π×2", "(2)×3") except after an evaluation, where
  it starts a new number.
- Percent: x% = x/100; as the right operand of + or - it is a share of the left operand (100+10% = 110,
  50-10% = 45, 8+5%% = 8.004); in × and ÷ it is x/100 (100×5% = 5, 200÷50% = 400); followed by an operand it is
  the remainder (5%2 = 1); a second % wraps: "(5%)%".
- Keys: digits replace a leading 0; "." adds a pending point (shown only with the next digit, rich token "7."), a
  second "." returns false; "+" "-" "×" "÷" "^" replace a trailing operator and return false after "(", a function
  or a sign; ± wraps the last operand in "(-x)" (and unwraps; "(3)" becomes "(-3)"), false after an operator;
  1/x wraps in "(1÷x)" (false for 1); x², x³ add "^2", "^3"; √ and ∛ prefix the last operand; x! and % are
  postfix; "(" opens "()" (auto closed, "3(" gives "3×()"), ")" is false when empty; functions applied to the last
  operand "sin(30)" or opened empty "sin()"; ee shows "2e" and takes digits ("2e3" = 2000); delete removes the
  last character or token (to "0" with prefersClear false); C removes the last operand ("12+C" gives "12+"),
  AC resets; prefersClear is true while there is something to clear.
- Trigonometry follows `assumeDegrees` (default from the option); sin 180° is exactly 0.
- `formattedResult` (no number formatter set): grouping, at least 3 fraction digits, 3 significant digits below 1
  (0.0523), scientific from 10 integer digits ("1.235×10¹⁰", superscript exponent).
- Rich expression: `^` is a token with empty text whose `right` holds the exponent; √ is "√" "(" operand ")";
  e is "e"; inverse functions are "asin" "acos" "atan"; log2 is "log".

Not copied yet: programmer mode (base 8/16, bit operations, shifts: those keys return false in base 10, as here),
units and currency, variables, graphing, the ranges beyond Decimal, rand's distribution, `postfixStack`.
