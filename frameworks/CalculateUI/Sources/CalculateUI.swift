// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import SwiftUI
import Foundation
import AppKit
import Calculate

func logDisplay(_ text: String) {
    if ProcessInfo.processInfo.environment["REEF_LOG_DISPLAY"] != nil { fputs("display: \(text)\n", stderr) }
}

public struct CalculateExpressionView: View {
    let expression: CalculateExpression
    let isEditable: Bool
    let fontSize: CGFloat?
    let height: CGFloat?
    let minFontSizeRatio: CGFloat?
    let textAlignment: TextAlignment
    let textColor: Color
    let truncationMode: Text.TruncationMode?
    let operationCallback: ((CalculateExpression.Operation) -> Void)?

    public init(expression: CalculateExpression, isEditable: Bool, height: CGFloat, minFontSizeRatio: CGFloat?,
                textAlignment: TextAlignment, textColor: Color, truncationMode: Text.TruncationMode?,
                operationCallback: ((CalculateExpression.Operation) -> Void)?) {
        self.expression = expression; self.isEditable = isEditable; self.height = height; self.fontSize = nil
        self.minFontSizeRatio = minFontSizeRatio; self.textAlignment = textAlignment; self.textColor = textColor
        self.truncationMode = truncationMode; self.operationCallback = operationCallback
    }

    public init(expression: CalculateExpression, isEditable: Bool, fontSize: CGFloat?, minFontSizeRatio: CGFloat?,
                textAlignment: TextAlignment, textColor: Color, truncationMode: Text.TruncationMode?,
                operationCallback: ((CalculateExpression.Operation) -> Void)?) {
        self.expression = expression; self.isEditable = isEditable; self.height = nil; self.fontSize = fontSize
        self.minFontSizeRatio = minFontSizeRatio; self.textAlignment = textAlignment; self.textColor = textColor
        self.truncationMode = truncationMode; self.operationCallback = operationCallback
    }

    public var body: some View {
        let _ = logDisplay(expression.displayText)
        Text(expression.displayText)
            .font(.system(size: fontSize ?? ((height ?? 48) * 0.8)))
            .foregroundColor(textColor)
            .multilineTextAlignment(textAlignment)
            .lineLimit(1)
            .minimumScaleFactor(minFontSizeRatio ?? 1)
            .frame(maxWidth: .infinity, alignment: textAlignment == .leading ? .leading : (textAlignment == .center ? .center : .trailing))
    }
}

public struct CalculateExpressionStackView: View {
    let expression: CalculateExpression
    let textAlignment: TextAlignment
    let textColor: Color
    let rowCount: Int
    let separatorColor: Color
    let operationCallback: ((CalculateExpression.Operation) -> Void)?

    public init(expression: CalculateExpression, textAlignment: TextAlignment, textColor: Color, rowCount: Int,
                separatorColor: Color, operationCallback: ((CalculateExpression.Operation) -> Void)?) {
        self.expression = expression; self.textAlignment = textAlignment; self.textColor = textColor
        self.rowCount = rowCount; self.separatorColor = separatorColor; self.operationCallback = operationCallback
    }

    public var body: some View {
        VStack(alignment: .trailing, spacing: 2) {
            ForEach(Array(expression.postfixStack.suffix(rowCount).enumerated()), id: \.offset) { _, e in
                Text(e.expression).foregroundColor(textColor)
            }
        }
    }
}

extension CalculateExpression {
    public enum CalculatorButton {
        case memoryClear, memoryRecall, memorySubtract, memoryAdd, second, degrees, radians
    }

    public struct Label: View {
        let text: String
        let fontSize: CGFloat
        public var caption: AttributedString? { nil }
        init(_ text: String, fontSize: CGFloat) { self.text = text; self.fontSize = fontSize }
        public var body: some View {
            Text(text).font(.system(size: fontSize * 1.4))
        }
    }

    public func label(for operation: Operation, fontSize: CGFloat) -> Label {
        return Label(operation.text, fontSize: fontSize)
    }

    public func label(for button: CalculatorButton, fontSize: CGFloat) -> Label {
        Label("\(button)", fontSize: fontSize)
    }

    public func copyToClipboard(overridePlaintext: String?) {
        let pb = NSPasteboard.general
        pb.clearContents()
        pb.setString(overridePlaintext ?? copyableExpression, forType: .string)
    }
}

extension StocksKitCurrencyCache {
    public struct Provider {
        public init() {}
        public var name: String { "Stocks" }
        public var destination: URL? { nil }
        public var logo: Image? { nil }
    }

    public var provider: Provider? { get async { nil } }
}
