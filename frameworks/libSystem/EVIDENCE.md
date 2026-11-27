# libSystem wrapper: evidence (macOS 26.7.2, x86_64, the Mac VM)

- `_FE_DFL_DISABLE_SSE_DENORMS_ENV` bytes: `7f 03 00 00 c0 9f 00 00` + 8 zero bytes (x87 control 0x037f, MXCSR 0x9fc0);
  `_FE_DFL_ENV`: MXCSR 0x1f80. Without the constant GarageBand's MADSP loaded MXCSR 0 (all exceptions unmasked) -> SIGFPE.
- `__cospi`: disassembled with lldb (libsystem_m): 2x = n + t, |t| <= 1/2, two degree-6 polynomials in t^2 (cos and sin
  of pi t / 2, coefficients read from libsystem_m), lane chosen by n + 1, sign by bit 1. Probe values match
  (cospi(0.25) = 0x1.6a09e667f3bccp-1, cospi(0.1) = 0x1.e6f0e134454ffp-1, 0.5 / 1.5 / 2.5 -> +0, odd integers -> -1).
- `__invert_d3`: lldb disassembly: cofactors with vfmsub (fused), determinant from the first column, multiplied by 1/det.
- `setlogmask(0)` answers 0 until a mask is set; the first set answers 0x3f (LOG_UPTO(LOG_NOTICE)).
- `strtonum`: "invalid"/EINVAL (no number, min > max), "too small"/"too large" with ERANGE, 0 on error.
- `timespec_get(&ts, TIME_UTC)` = 1, other bases 0. `pthread_qos_max_parallelism`: the CPU count; -1/EINVAL for an unknown
  QoS class or flags beyond 0x3. `thread_policy`: 0 for timesharing, round robin, FIFO; 28 (KERN_INVALID_POLICY) otherwise.
- `_dyld_objc_notify_register` (objc4 source, load_categories_nolock, _objc_realizeClassFromSwift): categories whose class
  is a Swift class stub (isa 1..15) wait in unattachedCategories and are attached by methodizeClass(cls, previously).
