// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
import Accessibility

// Attributes and an initializer that Reef's Accessibility lacks; the wrapper exports them under Accessibility's names.

extension AttributeScopes.AccessibilityAttributes {
    @frozen public enum AdjustedPitchAttribute: CodableAttributedStringKey, MarkdownDecodableAttributedStringKey, ObjectiveCConvertibleAttributedStringKey {
        public typealias Value = Double
        public typealias ObjectiveCValue = NSNumber
        public static var name: String { "NSAccessibilitySpeechPitch" }
        public static let markdownName: String = "adjustedPitch"
        public static func objectiveCValue(for value: Double) throws -> NSNumber { NSNumber(value: value) }
        public static func value(for object: NSNumber) throws -> Double { object.doubleValue }
    }

    @frozen public enum SpellOutAttribute: CodableAttributedStringKey, MarkdownDecodableAttributedStringKey {
        public typealias Value = Bool
        public static var name: String { "NSAccessibilitySpeechSpellOut" }
        public static let markdownName: String = "spellsOutCharacters"
    }

    @frozen public enum AnnouncementPriorityAttribute: CodableAttributedStringKey, MarkdownDecodableAttributedStringKey, ObjectiveCConvertibleAttributedStringKey {
        public typealias Value = AnnouncementPriority
        public typealias ObjectiveCValue = NSString
        public static var name: String { "NSAccessibilitySpeechAnnouncementPriority" }
        public static let markdownName: String = "announcementPriority"
        public static func objectiveCValue(for value: AnnouncementPriority) throws -> NSString { value.rawValue as NSString }
        public static func value(for object: NSString) throws -> AnnouncementPriority {
            AnnouncementPriority(rawValue: object as String) ?? .default
        }
        public enum AnnouncementPriority: String, Codable, Sendable {
            case low
            case `default`
            case high
        }
    }
}

extension AccessibilityNotification.Announcement {
    public init(_ announcement: AttributedString) {
        self.init(String(announcement.characters))
    }
}
