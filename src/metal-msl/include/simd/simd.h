// The simd types shared with programs' host code, as the shading language
// sees them.
#ifndef __REEF_SIMD
#define __REEF_SIMD

#include <metal_matrix>

#define __SIMD_TYPES(T)                                                                                              \
    typedef ::T##2 simd_##T##2;                                                                                      \
    typedef ::T##3 simd_##T##3;                                                                                      \
    typedef ::T##4 simd_##T##4;                                                                                      \
    typedef ::T##2 vector_##T##2;                                                                                    \
    typedef ::T##3 vector_##T##3;                                                                                    \
    typedef ::T##4 vector_##T##4;                                                                                    \
    namespace simd {                                                                                                 \
    using ::T##2;                                                                                                    \
    using ::T##3;                                                                                                    \
    using ::T##4;                                                                                                    \
    }

__SIMD_TYPES(char)
__SIMD_TYPES(uchar)
__SIMD_TYPES(short)
__SIMD_TYPES(ushort)
__SIMD_TYPES(int)
__SIMD_TYPES(uint)
__SIMD_TYPES(long)
__SIMD_TYPES(ulong)
__SIMD_TYPES(half)
__SIMD_TYPES(float)

#define __SIMD_MATRICES(T)                                                                                           \
    typedef ::T##2x2 simd_##T##2x2;                                                                                  \
    typedef ::T##3x3 simd_##T##3x3;                                                                                  \
    typedef ::T##4x4 simd_##T##4x4;                                                                                  \
    typedef ::T##2x3 simd_##T##2x3;                                                                                  \
    typedef ::T##2x4 simd_##T##2x4;                                                                                  \
    typedef ::T##3x2 simd_##T##3x2;                                                                                  \
    typedef ::T##3x4 simd_##T##3x4;                                                                                  \
    typedef ::T##4x2 simd_##T##4x2;                                                                                  \
    typedef ::T##4x3 simd_##T##4x3;                                                                                  \
    typedef ::T##2x2 matrix_##T##2x2;                                                                                \
    typedef ::T##3x3 matrix_##T##3x3;                                                                                \
    typedef ::T##4x4 matrix_##T##4x4;                                                                                \
    namespace simd {                                                                                                 \
    using ::T##2x2;                                                                                                  \
    using ::T##2x3;                                                                                                  \
    using ::T##2x4;                                                                                                  \
    using ::T##3x2;                                                                                                  \
    using ::T##3x3;                                                                                                  \
    using ::T##3x4;                                                                                                  \
    using ::T##4x2;                                                                                                  \
    using ::T##4x3;                                                                                                  \
    using ::T##4x4;                                                                                                  \
    }

__SIMD_MATRICES(float)
__SIMD_MATRICES(half)

namespace simd {
using ::__msl_make;
}

#endif
