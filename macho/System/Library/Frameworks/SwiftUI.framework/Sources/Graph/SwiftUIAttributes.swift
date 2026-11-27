// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// SwiftUIAttributes (Graph).

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

extension Foundation.AttributeScopes {
    public struct SwiftUIAttributes {
        public let font: Foundation.AttributeScopes.SwiftUIAttributes.FontAttribute
        public let foregroundColor: Foundation.AttributeScopes.SwiftUIAttributes.ForegroundColorAttribute
        public let backgroundColor: Foundation.AttributeScopes.SwiftUIAttributes.BackgroundColorAttribute
        public let strikethroughStyle: Foundation.AttributeScopes.SwiftUIAttributes.StrikethroughStyleAttribute
        public let underlineStyle: Foundation.AttributeScopes.SwiftUIAttributes.UnderlineStyleAttribute
        public let kern: Foundation.AttributeScopes.SwiftUIAttributes.KerningAttribute
        public let tracking: Foundation.AttributeScopes.SwiftUIAttributes.TrackingAttribute
        public let baselineOffset: Foundation.AttributeScopes.SwiftUIAttributes.BaselineOffsetAttribute
        public let foundation: Foundation.AttributeScopes.FoundationAttributes
        public enum FontAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum ForegroundColorAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum BackgroundColorAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum StrikethroughStyleAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum UnderlineStyleAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum KerningAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum TrackingAttribute {
            public static var name: Swift.String { _skeleton() }
        }
        public enum BaselineOffsetAttribute {
            public static var name: Swift.String { _skeleton() }
        }
    }
}

extension Foundation.AttributeScopes.SwiftUIAttributes: Foundation.AttributeScope {
}

extension Foundation.AttributeScopes.SwiftUIAttributes: Foundation.DecodingConfigurationProviding {
    public typealias DecodingConfiguration = Foundation.AttributeScopeCodableConfiguration
}

extension Foundation.AttributeScopes.SwiftUIAttributes: Foundation.EncodingConfigurationProviding {
    public typealias EncodingConfiguration = Foundation.AttributeScopeCodableConfiguration
}

extension Foundation.AttributeScopes.SwiftUIAttributes.FontAttribute: Foundation.AttributedStringKey {
    public typealias Value = SwiftUI.Font
}

extension Foundation.AttributeScopes.SwiftUIAttributes.ForegroundColorAttribute: Foundation.AttributedStringKey {
    public typealias Value = SwiftUI.Color
}

extension Foundation.AttributeScopes.SwiftUIAttributes.BackgroundColorAttribute: Foundation.AttributedStringKey {
    public typealias Value = SwiftUI.Color
}

extension Foundation.AttributeScopes.SwiftUIAttributes.StrikethroughStyleAttribute: Foundation.AttributedStringKey {
    public typealias Value = SwiftUI.Text.LineStyle
}

extension Foundation.AttributeScopes.SwiftUIAttributes.UnderlineStyleAttribute: Foundation.AttributedStringKey {
    public typealias Value = SwiftUI.Text.LineStyle
}

extension Foundation.AttributeScopes.SwiftUIAttributes.KerningAttribute: Foundation.AttributedStringKey {
    public typealias Value = CoreGraphics.CGFloat
}

extension Foundation.AttributeScopes.SwiftUIAttributes.TrackingAttribute: Foundation.AttributedStringKey {
    public typealias Value = CoreGraphics.CGFloat
}

extension Foundation.AttributeScopes.SwiftUIAttributes.BaselineOffsetAttribute: Foundation.AttributedStringKey {
    public typealias Value = CoreGraphics.CGFloat
}
