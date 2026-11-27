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
    public init() { _skeleton() }
    func isSame(`as` a0: SwiftUI.EnvironmentValues, customOnly a1: Swift.Bool) -> Swift.Bool { _skeleton() }
    public var tintColor: SwiftUI.Color? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var colorScheme: SwiftUI.ColorScheme {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var controlActiveState: SwiftUI.ControlActiveState {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var isEnabled: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var controlSize: SwiftUI.ControlSize {
        get { _skeleton() }
        set { _skeleton() }
    }
    var labelsHidden: Swift.Bool { _skeleton() }
    var toggleStyleBox: Any? { _skeleton() }
    public var font: SwiftUI.Font? {
        get { _skeleton() }
        set { _skeleton() }
    }
    var buttonSizing: SwiftUI.ButtonSizing { _skeleton() }
    public var _buttonBorderShape: SwiftUI.ButtonBorderShape {
        get { _skeleton() }
        set { _skeleton() }
    }
    var horizontalRadioGroup: Swift.Bool { _skeleton() }
    var progressStyle: Any? { _skeleton() }
    public var menuIndicatorVisibility: SwiftUI.Visibility {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var foregroundColor: SwiftUI.Color? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var multilineTextAlignment: SwiftUI.TextAlignment {
        get { _skeleton() }
        set { _skeleton() }
    }
    var toggleButtonIsOn: Swift.Bool { _skeleton() }
    var labelStyle: Any? { _skeleton() }
    public var imageScale: SwiftUI.Image.Scale {
        get { _skeleton() }
        set { _skeleton() }
    }
    var labelIconToTitleSpacing: CoreGraphics.CGFloat? { _skeleton() }
    public var openURL: SwiftUI.OpenURLAction {
        get { _skeleton() }
        set { _skeleton() }
    }
    public subscript<A>(_ a0: A.Type) -> A.Value where A: SwiftUI.EnvironmentKey {
        get { _skeleton() }
        set { _skeleton() }
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
        get { _skeleton() }
        set { _skeleton() }
    }
    var scrollContentBackgroundVisibility: SwiftUI.Visibility { _skeleton() }
    var isSheetContent: Swift.Bool { _skeleton() }
    public var lineLimit: Swift.Int? {
        get { _skeleton() }
        set { _skeleton() }
    }
    var scrollAnchors: SwiftUI.ScrollAnchors { _skeleton() }
    var scrollContentMargins: SwiftUI.EdgeInsets { _skeleton() }
    var scrollTargetBehavior: (any SwiftUI.ScrollTargetBehavior)? { _skeleton() }
    var verticalScrollIndicators: SwiftUI.ScrollIndicatorVisibility { _skeleton() }
    var horizontalScrollIndicators: SwiftUI.ScrollIndicatorVisibility { _skeleton() }
    var fileDialogURLEnabled: (Foundation.Predicate<(Foundation.URL)>)? { _skeleton() }
    var textScale: SwiftUI.Text.Scale? { _skeleton() }
    public var layoutDirection: SwiftUI.LayoutDirection {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var lineSpacing: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var allowsTightening: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var truncationMode: SwiftUI.Text.TruncationMode {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var textCase: SwiftUI.Text.Case? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var displayScale: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var minimumScaleFactor: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    subscript<A>(_object a0: Swift.ObjectIdentifier) -> A? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var description: Swift.String { _skeleton() }
    public var contentTransition: SwiftUI.ContentTransition {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var appearsActive: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var tint: SwiftUI.AnyShapeStyle? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accessibilityReduceMotion: Swift.Bool { _skeleton() }
    public var pixelLength: CoreGraphics.CGFloat { _skeleton() }
    public var dynamicTypeSize: SwiftUI.DynamicTypeSize {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var redactionReasons: SwiftUI.RedactionReasons {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var timeZone: Foundation.TimeZone {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accessibilityShowButtonShapes: Swift.Bool { _skeleton() }
    public var _accessibilityShowButtonShapes: Swift.Bool {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var calendar: Foundation.Calendar {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var undoManager: UndoManager? { _skeleton() }
    public var locale: Foundation.Locale {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var textContentType: Swift.String? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var symbolRenderingMode: SwiftUI.SymbolRenderingMode? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var symbolColorRenderingMode: SwiftUI.SymbolColorRenderingMode? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accessibilityDifferentiateWithoutColor: Swift.Bool { _skeleton() }
    public var sidebarRowSize: SwiftUI.SidebarRowSize {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var badgeProminence: SwiftUI.BadgeProminence {
        get { _skeleton() }
        set { _skeleton() }
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
        get { _skeleton() }
        set { _skeleton() }
    }
    public var accessibilityVoiceOverEnabled: Swift.Bool { _skeleton() }
    public var defaultMinListHeaderHeight: CoreGraphics.CGFloat? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var defaultMinListRowHeight: CoreGraphics.CGFloat {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var dismiss: SwiftUI.DismissAction { _skeleton() }
    public var isPresented: Swift.Bool { _skeleton() }
    public var isSearching: Swift.Bool { _skeleton() }
    public var presentationMode: SwiftUI.Binding<SwiftUI.PresentationMode> { _skeleton() }
    public var horizontalSizeClass: SwiftUI.UserInterfaceSizeClass? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var verticalSizeClass: SwiftUI.UserInterfaceSizeClass? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var legibilityWeight: SwiftUI.LegibilityWeight? {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var menuOrder: SwiftUI.MenuOrder {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var scenePhase: SwiftUI.ScenePhase {
        get { _skeleton() }
        set { _skeleton() }
    }
    public var sizeCategory: SwiftUI.ContentSizeCategory {
        get { _skeleton() }
        set { _skeleton() }
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
