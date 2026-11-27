// SPDX-FileCopyrightText: 2026 Reion Wong <reionwong@gmail.com>
// SPDX-License-Identifier: BSD-3-Clause
//
// Reconstructed from the published binary.
// The stored fields of a type through the Swift runtime (what the standard library's _forEachField does).

/// the stored fields of a struct or tuple type a0 (name, byte offset, type), as the standard library's
/// _forEachField(of:options: .ignoreUnknown) gives them: its interface is not public, so through the runtime functions
/// it calls; false when a0 is a class or a1 stops early
func _reefForEachField(of a0: Any.Type, _ a1: (UnsafePointer<CChar>, Swift.Int, Any.Type) -> Swift.Bool) -> Swift.Bool {
    if a0 is AnyClass { return false }
    let count = _reefFieldCount(a0)
    if count <= 0 { return true }
    for index in 0..<count {
        let offset = _reefFieldOffset(a0, index)
        var field = SwiftUI._ReefFieldMetadata()
        let type = _reefFieldType(a0, index, &field)
        defer { field.freeFunc?(field.name) }
        guard let name = field.name else { continue }
        if !a1(name, offset, type) { return false }
    }
    return true
}

/// the kind word of a type's metadata (0x200 struct, 0x303 existential ...)
func _reefMetadataKind(_ a0: Any.Type) -> Swift.Int {
    unsafeBitCast(a0, to: UnsafePointer<Swift.Int>.self).pointee
}

/// the size of a value of type a0 (its value witness table's size)
func _reefSize(of a0: Any.Type) -> Swift.Int {
    func size<T>(_: T.Type) -> Swift.Int { MemoryLayout<T>.size }
    return _openExistential(a0, do: size)
}

/// _FieldReflectionMetadata of the Swift runtime
struct _ReefFieldMetadata {
    var name: UnsafePointer<CChar>? = nil
    var freeFunc: (@convention(c) (UnsafePointer<CChar>?) -> Void)? = nil
    var isStrong: Swift.Bool = true
    var isVar: Swift.Bool = false
}
@_silgen_name("swift_reflectionMirror_recursiveCount")
func _reefFieldCount(_ a0: Any.Type) -> Swift.Int
@_silgen_name("swift_reflectionMirror_recursiveChildOffset")
func _reefFieldOffset(_ a0: Any.Type, _ a1: Swift.Int) -> Swift.Int
@_silgen_name("swift_reflectionMirror_recursiveChildMetadata")
func _reefFieldType(_ a0: Any.Type, _ a1: Swift.Int, _ a2: UnsafeMutablePointer<SwiftUI._ReefFieldMetadata>) -> Any.Type
