// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

import Foundation
import Observation

// SwiftData as far as Calculator uses it. Storage is in memory for now (milestone 1).

public protocol SchemaProperty {}

public protocol BackingData<Model>: AnyObject {
    associatedtype Model: PersistentModel
    func setValue<T: Encodable>(forKey: KeyPath<Model, T>, to: T)
    func getValue<T: Decodable>(forKey: KeyPath<Model, T>) -> T
}

public protocol PersistentModel: AnyObject, Observable, Hashable, Identifiable {
    associatedtype Root: PersistentModel = Self
    static var schemaMetadata: [Schema.PropertyMetadata] { get }
    var persistentBackingData: any BackingData<Self> { get set }
    init(backingData: any BackingData<Self>)
}

final class MemoryBackingData<M: PersistentModel>: BackingData {
    typealias Model = M
    var values: [AnyKeyPath: Any] = [:]
    func setValue<T: Encodable>(forKey key: KeyPath<M, T>, to value: T) { values[key] = value }
    func getValue<T: Decodable>(forKey key: KeyPath<M, T>) -> T {
        if let v = values[key] as? T { return v }
        for p in M.schemaMetadata where p.keypath == key {
            if let d = p.defaultValue as? T { return d }
        }
        fatalError("SwiftData: no value for key path")
    }
}

extension PersistentModel {
    public static func createBackingData<T: PersistentModel>() -> some BackingData<T> {
        MemoryBackingData<T>()
    }
    public static func == (lhs: Self, rhs: Self) -> Bool { lhs === rhs }
    public func hash(into hasher: inout Hasher) { hasher.combine(ObjectIdentifier(self)) }
    public func getValue<T: Decodable>(forKey key: KeyPath<Self, T>) -> T {
        persistentBackingData.getValue(forKey: key)
    }
    public func setValue<T: Encodable>(forKey key: KeyPath<Self, T>, to value: T) {
        persistentBackingData.setValue(forKey: key, to: value)
    }
}

open class Schema {
    public struct Version: Hashable, Comparable, Sendable {
        public var major: Int, minor: Int, patch: Int
        public init(_ major: Int, _ minor: Int, _ patch: Int) { self.major = major; self.minor = minor; self.patch = patch }
        public static func < (a: Version, b: Version) -> Bool { (a.major, a.minor, a.patch) < (b.major, b.minor, b.patch) }
    }
    public struct PropertyMetadata {
        public var name: String
        public var keypath: AnyKeyPath
        public var defaultValue: Any?
        public var metadata: (any SchemaProperty)?
        public init(name: String, keypath: AnyKeyPath, defaultValue: Any? = nil, metadata: (any SchemaProperty)? = nil) {
            self.name = name; self.keypath = keypath; self.defaultValue = defaultValue; self.metadata = metadata
        }
    }
    public let models: [any PersistentModel.Type]
    public let version: Version
    public init(_ models: [any PersistentModel.Type], version: Version = Version(1, 0, 0)) {
        self.models = models; self.version = version
    }
}

public protocol VersionedSchema {
    static var versionIdentifier: Schema.Version { get }
    static var models: [any PersistentModel.Type] { get }
}

public protocol SchemaMigrationPlan {
    static var schemas: [any VersionedSchema.Type] { get }
}

public struct ModelConfiguration: Sendable {
    public struct GroupContainer: Sendable {
        public static var automatic: GroupContainer { GroupContainer() }
        public static var none: GroupContainer { GroupContainer() }
    }
    public struct CloudKitDatabase: Sendable {
        public static var automatic: CloudKitDatabase { CloudKitDatabase() }
        public static var none: CloudKitDatabase { CloudKitDatabase() }
    }
    public var name: String?
    public var schema: Schema?
    public var isStoredInMemoryOnly: Bool
    public var allowsSave: Bool
    public init(_ name: String? = nil, schema: Schema? = nil, isStoredInMemoryOnly: Bool = false, allowsSave: Bool = true,
                groupContainer: GroupContainer = .automatic, cloudKitDatabase: CloudKitDatabase = .automatic) {
        self.name = name; self.schema = schema; self.isStoredInMemoryOnly = isStoredInMemoryOnly; self.allowsSave = allowsSave
    }
}

public struct FetchDescriptor<T: PersistentModel> {
    public var predicate: Predicate<T>?
    public init(predicate: Predicate<T>? = nil, sortBy: [SortDescriptor<T>] = []) {
        self.predicate = predicate
    }
}

open class ModelContainer: @unchecked Sendable {
    public let schema: Schema
    public let configurations: [ModelConfiguration]
    public init(for schema: Schema, migrationPlan: (any SchemaMigrationPlan.Type)? = nil, configurations: ModelConfiguration...) throws {
        self.schema = schema; self.configurations = configurations
    }
}

open class ModelContext {
    public static let didSave = Notification.Name("NSManagedObjectContextDidSaveNotification")
    public let container: ModelContainer
    var inserted: [AnyObject] = []
    public init(_ container: ModelContainer) { self.container = container }
    open func insert<T: PersistentModel>(_ model: T) { inserted.append(model) }
    open func save() throws {}
    open func fetch<T: PersistentModel>(_ descriptor: FetchDescriptor<T>) throws -> [T] { inserted.compactMap { $0 as? T } }
    open func delete<T: PersistentModel>(model: T.Type, where predicate: Predicate<T>? = nil, includeSubclasses: Bool = true) throws {
        inserted.removeAll { $0 is T }
    }
}

public protocol ModelExecutor: SerialExecutor {
    var modelContext: ModelContext { get }
}

public final class DefaultSerialModelExecutor: ModelExecutor {
    public let modelContext: ModelContext
    let queue = DispatchQueue(label: "SwiftData.ModelExecutor")
    public init(modelContext: ModelContext) { self.modelContext = modelContext }
    public func enqueue(_ job: consuming ExecutorJob) {
        let unowned = UnownedJob(job)
        queue.async { unowned.runSynchronously(on: self.asUnownedSerialExecutor()) }
    }
}

public protocol ModelActor: Actor {
    nonisolated var modelExecutor: any ModelExecutor { get }
    nonisolated var modelContainer: ModelContainer { get }
}

extension ModelActor {
    public var modelContext: ModelContext { modelExecutor.modelContext }
    public nonisolated var unownedExecutor: UnownedSerialExecutor { modelExecutor.asUnownedSerialExecutor() }
}
