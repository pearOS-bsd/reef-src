# Foundation wrapper: evidence (macOS 26.7.2, x86_64, the Mac VM)

- `+setKeys:triggerChangeNotificationsForDependentKey:` (lldb disassembly + probe): NSKeyValueOldStyleDependenciesByClass
  [class][dependentKey] = [[NSSet alloc] initWithArray:keys] under a lock; per class, not inherited (a subclass answers
  an empty set), a second call replaces. `+_keysForValuesAffectingValueForKey:` copies the keys without a dot into a new
  NSMutableSet. `+keyPathsForValuesAffectingValueForKey:` invokes `+keyPathsForValuesAffecting<Key>` (first UTF-8 byte
  upper-cased, class_getClassMethod + method_invoke) when the class has it, else the old style keys; a key with a NUL
  raises NSInvalidArgumentException. KVO probe: with y as the only dependency, changing y notifies sum, x does not.
- `-[NSKeyedUnarchiver versionForClassName:]`: the class's current `+version` (a class with version 7 answers 7, NSString 1),
  NSNotFound for a class that does not exist.
- `-[NSMutableArray removeObject:]` on Apple does not message a freed object when the array held the last reference to the
  argument (GarageBand removes a dictionary it got by enumerating the same array).
