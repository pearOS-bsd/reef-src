// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// EnvironmentValues (Environment).

import Foundation
import AppKit
import CoreGraphics
import CoreText
import Combine
import Observation
import CoreTransferable
import UniformTypeIdentifiers
import Symbols
import DeveloperToolsSupport
import Metal
import os

public struct EnvironmentValues {
    var values: [Swift.ObjectIdentifier: Any]
    public init() { values = [:] }
    /// the same values (with customOnly: the same values of the app's own keys)
    func isSame(`as` a0: SwiftUI.EnvironmentValues, customOnly a1: Swift.Bool) -> Swift.Bool {
        func custom(_ v: [Swift.ObjectIdentifier: Any]) -> [Swift.ObjectIdentifier: Any] {
            v.filter { ($0.value as? any SwiftUI.EnvironmentBoxComparing)?.isCustom ?? false }
        }
        let mine = a1 ? custom(values) : values
        let theirs = a1 ? custom(a0.values) : a0.values
        guard mine.count == theirs.count else { return false }
        for (key, value) in mine {
            guard let other = theirs[key], let box = value as? any SwiftUI.EnvironmentBoxComparing,
                  box.sameValue(as: other) else { return false }
        }
        return true
    }
    public var tintColor: SwiftUI.Color? {
        get { self[SwiftUI.TintColorKey.self] }
        set { self[SwiftUI.TintColorKey.self] = newValue }
    }
    public var colorScheme: SwiftUI.ColorScheme {
        get { self[SwiftUI.ColorSchemeKey.self] }
        set { self[SwiftUI.ColorSchemeKey.self] = newValue }
    }
    public var controlActiveState: SwiftUI.ControlActiveState {
        get { self[SwiftUI.ControlActiveStateKey.self] }
        set { self[SwiftUI.ControlActiveStateKey.self] = newValue }
    }
    public var isEnabled: Swift.Bool {
        get { self[SwiftUI.EnabledKey.self] }
        set { self[SwiftUI.EnabledKey.self] = newValue }
    }
    public var controlSize: SwiftUI.ControlSize {
        get { self[SwiftUI.ControlSizeKey.self] }
        set { self[SwiftUI.ControlSizeKey.self] = newValue }
    }
    var labelsHidden: Swift.Bool { _skeleton() }
    var toggleStyleBox: Any? { _skeleton() }
    public var font: SwiftUI.Font? {
        get { self[SwiftUI.FontKey.self] }
        set { self[SwiftUI.FontKey.self] = newValue }
    }
    var buttonSizing: SwiftUI.ButtonSizing { _skeleton() }
    public var _buttonBorderShape: SwiftUI.ButtonBorderShape {
        get { _skeleton() }
        set { _skeleton() }
    }
    var horizontalRadioGroup: Swift.Bool { _skeleton() }
    var progressStyle: Any? { _skeleton() }
    public var menuIndicatorVisibility: SwiftUI.Visibility {
        get { self[SwiftUI.MenuIndicatorKey.self] }
        set { self[SwiftUI.MenuIndicatorKey.self] = newValue }
    }
    public var foregroundColor: SwiftUI.Color? {
        get { self[SwiftUI.ForegroundColorKey.self] }
        set { self[SwiftUI.ForegroundColorKey.self] = newValue }
    }
    public var multilineTextAlignment: SwiftUI.TextAlignment {
        get { self[SwiftUI.TextAlignmentKey.self] }
        set { self[SwiftUI.TextAlignmentKey.self] = newValue }
    }
    var toggleButtonIsOn: Swift.Bool { _skeleton() }
    var labelStyle: Any? { _skeleton() }
    public var imageScale: SwiftUI.Image.Scale {
        get { self[SwiftUI.ImageScaleKey.self] }
        set { self[SwiftUI.ImageScaleKey.self] = newValue }
    }
    var labelIconToTitleSpacing: CoreGraphics.CGFloat? { _skeleton() }
    public var openURL: SwiftUI.OpenURLAction {
        get { _skeleton() }
        set { _skeleton() }
    }
    public subscript<A>(_ a0: A.Type) -> A.Value where A: SwiftUI.EnvironmentKey {
        get {
            if let box = values[ObjectIdentifier(a0)] as? SwiftUI.EnvironmentBox<A.Value> { return box.value }
            return A.defaultValue
        }
        set {
            values[ObjectIdentifier(a0)] = SwiftUI.EnvironmentBox(newValue, isCustom: SwiftUI.EnvironmentKeyOrigin.isCustom(a0))
        }
    }
    func pushStyle(_ a0: Any, kind a1: SwiftUI.StyleKind) { _skeleton() }
    func popStyle(_ a0: SwiftUI.StyleKind) { _skeleton() }
    var symbolRenderingModeValue: SwiftUI.SymbolRenderingMode? { _skeleton() }
    var lineLimitMinimum: Swift.Int? { _skeleton() }
    var alternatesRowBackgrounds: Swift.Bool? { _skeleton() }
    var tableColumnHeadersHidden: Swift.Bool { _skeleton() }
    var textBold: Swift.Bool { _skeleton() }
    var textItalic: Swift.Bool { _skeleton() }
    var textMonospacedDigit: Swift.Bool { _skeleton() }
    var textWeight: SwiftUI.Font.Weight? { _skeleton() }
    var textDesign: SwiftUI.Font.Design? { _skeleton() }
    var textFieldStyle: Any? { _skeleton() }
    var textSelectable: Swift.Bool { _skeleton() }
    var dimsDisabledText: Swift.Bool { _skeleton() }
    public var symbolVariants: SwiftUI.SymbolVariants {
        get { self[SwiftUI.SymbolVariantsKey.self] }
        set { self[SwiftUI.SymbolVariantsKey.self] = newValue }
    }
    var scrollContentBackgroundVisibility: SwiftUI.Visibility { _skeleton() }
    var isSheetContent: Swift.Bool { _skeleton() }
    public var lineLimit: Swift.Int? {
        get { self[SwiftUI.LineLimitKey.self] }
        set { self[SwiftUI.LineLimitKey.self] = newValue }
    }
    var scrollAnchors: SwiftUI.ScrollAnchors { _skeleton() }
    var scrollContentMargins: SwiftUI.EdgeInsets { _skeleton() }
    var scrollTargetBehavior: (any SwiftUI.ScrollTargetBehavior)? { _skeleton() }
    var verticalScrollIndicators: SwiftUI.ScrollIndicatorVisibility { _skeleton() }
    var horizontalScrollIndicators: SwiftUI.ScrollIndicatorVisibility { _skeleton() }
    var fileDialogURLEnabled: (Foundation.Predicate<(Foundation.URL)>)? { _skeleton() }
    var textScale: SwiftUI.Text.Scale? { _skeleton() }
    public var layoutDirection: SwiftUI.LayoutDirection {
        get { self[SwiftUI.LayoutDirectionKey.self] }
        set { self[SwiftUI.LayoutDirectionKey.self] = newValue }
    }
    public var lineSpacing: CoreGraphics.CGFloat {
        get { self[SwiftUI.LineSpacingKey.self] }
        set { self[SwiftUI.LineSpacingKey.self] = newValue }
    }
    public var allowsTightening: Swift.Bool {
        get { self[SwiftUI.AllowsTighteningKey.self] }
        set { self[SwiftUI.AllowsTighteningKey.self] = newValue }
    }
    public var truncationMode: SwiftUI.Text.TruncationMode {
        get { self[SwiftUI.TruncationModeKey.self] }
        set { self[SwiftUI.TruncationModeKey.self] = newValue }
    }
    public var textCase: SwiftUI.Text.Case? {
        get { self[SwiftUI.TextCaseKey.self] }
        set { self[SwiftUI.TextCaseKey.self] = newValue }
    }
    public var displayScale: CoreGraphics.CGFloat {
        get { self[SwiftUI.DisplayScaleKey.self] }
        set { self[SwiftUI.DisplayScaleKey.self] = newValue }
    }
    public var minimumScaleFactor: CoreGraphics.CGFloat {
        get { self[SwiftUI.MinimumScaleFactorKey.self] }
        set { self[SwiftUI.MinimumScaleFactorKey.self] = newValue }
    }
    subscript<A>(_object a0: Swift.ObjectIdentifier) -> A? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var description: Swift.String { _skeleton() }
    public var contentTransition: SwiftUI.ContentTransition {
        get { self[SwiftUI.ContentTransitionKey.self] }
        set { self[SwiftUI.ContentTransitionKey.self] = newValue }
    }
    public var appearsActive: Swift.Bool {
        get { self[SwiftUI.AppearsActiveKey.self] }
        set { self[SwiftUI.AppearsActiveKey.self] = newValue }
    }
    public var tint: SwiftUI.AnyShapeStyle? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accessibilityReduceMotion: Swift.Bool { _skeleton() }
    public var pixelLength: CoreGraphics.CGFloat { _skeleton() }
    public var dynamicTypeSize: SwiftUI.DynamicTypeSize {
        get { self[SwiftUI.DynamicTypeSizeKey.self] }
        set { self[SwiftUI.DynamicTypeSizeKey.self] = newValue }
    }
    public var redactionReasons: SwiftUI.RedactionReasons {
        get { self[SwiftUI.RedactionReasonsKey.self] }
        set { self[SwiftUI.RedactionReasonsKey.self] = newValue }
    }
    public var timeZone: Foundation.TimeZone {
        get { self[SwiftUI.TimeZoneKey.self] }
        set { self[SwiftUI.TimeZoneKey.self] = newValue }
    }
    public var accessibilityShowButtonShapes: Swift.Bool { _skeleton() }
    public var _accessibilityShowButtonShapes: Swift.Bool {
        get { self[SwiftUI.ShowButtonShapesKey.self] }
        set { self[SwiftUI.ShowButtonShapesKey.self] = newValue }
    }
    public var calendar: Foundation.Calendar {
        get { self[SwiftUI.CalendarKey.self] }
        set { self[SwiftUI.CalendarKey.self] = newValue }
    }
    public var undoManager: UndoManager? { _skeleton() }
    public var locale: Foundation.Locale {
        get { self[SwiftUI.LocaleKey.self] }
        set { self[SwiftUI.LocaleKey.self] = newValue }
    }
    public var textContentType: Swift.String? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var symbolRenderingMode: SwiftUI.SymbolRenderingMode? {
        get { self[SwiftUI.SymbolRenderingModeKey.self] }
        set { self[SwiftUI.SymbolRenderingModeKey.self] = newValue }
    }
    public var symbolColorRenderingMode: SwiftUI.SymbolColorRenderingMode? {
        get { self[SwiftUI.SymbolColorRenderingModeKey.self] }
        set { self[SwiftUI.SymbolColorRenderingModeKey.self] = newValue }
    }
    public var accessibilityDifferentiateWithoutColor: Swift.Bool { _skeleton() }
    public var sidebarRowSize: SwiftUI.SidebarRowSize {
        get { self[SwiftUI.SidebarRowSizeKey.self] }
        set { self[SwiftUI.SidebarRowSizeKey.self] = newValue }
    }
    public var badgeProminence: SwiftUI.BadgeProminence {
        get { self[SwiftUI.BadgeProminenceKey.self] }
        set { self[SwiftUI.BadgeProminenceKey.self] = newValue }
    }
    public var resetFocus: SwiftUI.ResetFocusAction { _skeleton() }
    public var accessibilityReduceTransparency: Swift.Bool { _skeleton() }
    public subscript<A>(_ a0: A.Type) -> A? where A: AnyObject, A: Observation.Observable {
        get { _skeleton() }
        set { _skeleton() }
    }
    subscript<A>(observable a0: Swift.ObjectIdentifier) -> A? where A: AnyObject {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accentColor: SwiftUI.Color? {
        get { self[SwiftUI.AccentColorKey.self] }
        set { self[SwiftUI.AccentColorKey.self] = newValue }
    }
    public var accessibilityVoiceOverEnabled: Swift.Bool { _skeleton() }
    public var defaultMinListHeaderHeight: CoreGraphics.CGFloat? {
        get { self[SwiftUI.MinListHeaderHeightKey.self] }
        set { self[SwiftUI.MinListHeaderHeightKey.self] = newValue }
    }
    public var defaultMinListRowHeight: CoreGraphics.CGFloat {
        get { self[SwiftUI.MinListRowHeightKey.self] }
        set { self[SwiftUI.MinListRowHeightKey.self] = newValue }
    }
    public var dismiss: SwiftUI.DismissAction { _skeleton() }
    public var isPresented: Swift.Bool { _skeleton() }
    public var isSearching: Swift.Bool { _skeleton() }
    public var presentationMode: SwiftUI.Binding<SwiftUI.PresentationMode> { _skeleton() }
    public var horizontalSizeClass: SwiftUI.UserInterfaceSizeClass? {
        get { self[SwiftUI.HorizontalSizeClassKey.self] }
        set { self[SwiftUI.HorizontalSizeClassKey.self] = newValue }
    }
    public var verticalSizeClass: SwiftUI.UserInterfaceSizeClass? {
        get { self[SwiftUI.VerticalSizeClassKey.self] }
        set { self[SwiftUI.VerticalSizeClassKey.self] = newValue }
    }
    public var legibilityWeight: SwiftUI.LegibilityWeight? {
        get { self[SwiftUI.LegibilityWeightKey.self] }
        set { self[SwiftUI.LegibilityWeightKey.self] = newValue }
    }
    public var menuOrder: SwiftUI.MenuOrder {
        get { self[SwiftUI.MenuOrderKey.self] }
        set { self[SwiftUI.MenuOrderKey.self] = newValue }
    }
    public var scenePhase: SwiftUI.ScenePhase {
        get { self[SwiftUI.ScenePhaseKey.self] }
        set { self[SwiftUI.ScenePhaseKey.self] = newValue }
    }
    public var sizeCategory: SwiftUI.ContentSizeCategory {
        get { self[SwiftUI.SizeCategoryKey.self] }
        set { self[SwiftUI.SizeCategoryKey.self] = newValue }
    }
    subscript<A>(requiredObservable a0: Swift.ObjectIdentifier) -> A where A: AnyObject { _skeleton() }
    public var openWindow: SwiftUI.OpenWindowAction { _skeleton() }
    public var openSettings: SwiftUI.OpenSettingsAction { _skeleton() }
    public var dismissWindow: SwiftUI.DismissWindowAction { _skeleton() }
    public var tabBarPlacement: SwiftUI.TabBarPlacement? { _skeleton() }
    public var colorSchemeContrast: SwiftUI.ColorSchemeContrast { _skeleton() }
}

extension SwiftUI.EnvironmentValues: Swift.CustomStringConvertible {
}
