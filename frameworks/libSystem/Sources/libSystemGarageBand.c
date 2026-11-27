// SPDX-FileCopyrightText: 2026 Alexandru Balan
// SPDX-License-Identifier: BSD-2-Clause

// libSystem functions and constants that GarageBand imports and Reef's libSystem lacks (macOS 26 values and
// behaviour observed on the Mac: frameworks/libSystem/EVIDENCE.md).

#include <errno.h>
#include <fenv.h>
#include <fts.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/attr.h>
#include <sys/fsgetpath.h>
#include <time.h>
#include <unistd.h>
#include <simd/simd.h>

// x87 control word, status word, MXCSR: the default environment with SSE denormals flushed and treated as zero
const fenv_t _FE_DFL_DISABLE_SSE_DENORMS_ENV = {0x037f, 0x0000, 0x9fc0, {0}};

const simd_double3x3 matrix_identity_double3x3 = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};

int __isinfd(double x) { return isinf(x) ? 1 : 0; }
int __isinff(float x) { return isinf(x) ? 1 : 0; }
int __isnand(double x) { return isnan(x) ? 1 : 0; }

// cos(pi x): 2x = n + t with |t| <= 1/2, the polynomials of Apple's libm for cos(pi t / 2) and sin(pi t / 2)
double __cospi(double x) {
    static const double c[7] = {-6.3294004511091526E-9, 4.7106784363023323E-7, -2.5202039028895381E-5, 9.1926027454695687E-4,
                                -2.086348076334078E-2, 0.25366950790104781, -1.2337005501361697};
    static const double s[7] = {5.6339384830096129E-8, -3.5986433882446739E-6, 1.6044115075210889E-4, -4.6817541323422619E-3,
                                7.9692626246043077E-2, -0.64596409750624428, 1.5707963267948966};
    const double ax = fabs(x);
    if (isnan(x) || isinf(x))
        return x - x;
    if (ax >= 0x1p53)
        return 1.0;
    int64_t n;
    double t;
    if (ax >= 0x1p51) {
        n = (int64_t)(x + x);
        t = 0;
    } else {
        const double twice = x + x;
        n = (int64_t)(twice + copysign(0.5, x));
        t = twice - (double)n;
    }
    if (t == 0) {
        if (n & 1)
            return 0.0;
        return (n & 2) ? -1.0 : 1.0;
    }
    const double u = t * t;
    double pc = c[0], ps = s[0];
    for (int i = 1; i < 7; i++) {
        pc = pc * u + c[i];
        ps = ps * u + s[i];
    }
    const int64_t m = n + 1;
    double r = (m & 1) ? u * pc + 1.0 : t * ps + 0.0;
    return (m & 2) ? -r : r;
}

// compiler-rt
double __powidf2(double a, int b) {
    const int reciprocal = b < 0;
    double r = 1;
    for (;;) {
        if (b & 1)
            r *= a;
        b /= 2;
        if (b == 0)
            break;
        a *= a;
    }
    return reciprocal ? 1 / r : r;
}

float __powisf2(float a, int b) {
    const int reciprocal = b < 0;
    float r = 1;
    for (;;) {
        if (b & 1)
            r *= a;
        b /= 2;
        if (b == 0)
            break;
        a *= a;
    }
    return reciprocal ? 1 / r : r;
}

// C11 Annex G complex division
#pragma STDC FP_CONTRACT OFF
double _Complex __divdc3(double a, double b, double c, double d) {
    int ilogbw = 0;
    const double logbw = logb(fmax(fabs(c), fabs(d)));
    if (isfinite(logbw)) {
        ilogbw = (int)logbw;
        c = scalbn(c, -ilogbw);
        d = scalbn(d, -ilogbw);
    }
    const double denom = c * c + d * d;
    double x = scalbn((a * c + b * d) / denom, -ilogbw);
    double y = scalbn((b * c - a * d) / denom, -ilogbw);
    if (isnan(x) && isnan(y)) {
        if (denom == 0.0 && (!isnan(a) || !isnan(b))) {
            x = copysign(INFINITY, c) * a;
            y = copysign(INFINITY, c) * b;
        } else if ((isinf(a) || isinf(b)) && isfinite(c) && isfinite(d)) {
            a = copysign(isinf(a) ? 1.0 : 0.0, a);
            b = copysign(isinf(b) ? 1.0 : 0.0, b);
            x = INFINITY * (a * c + b * d);
            y = INFINITY * (b * c - a * d);
        } else if (isinf(logbw) && logbw > 0.0 && isfinite(a) && isfinite(b)) {
            c = copysign(isinf(c) ? 1.0 : 0.0, c);
            d = copysign(isinf(d) ? 1.0 : 0.0, d);
            x = 0.0 * (a * c + b * d);
            y = 0.0 * (b * c - a * d);
        }
    }
    return __builtin_complex(x, y);
}

float _Complex __divsc3(float a, float b, float c, float d) {
    int ilogbw = 0;
    const float logbw = logbf(fmaxf(fabsf(c), fabsf(d)));
    if (isfinite(logbw)) {
        ilogbw = (int)logbw;
        c = scalbnf(c, -ilogbw);
        d = scalbnf(d, -ilogbw);
    }
    const float denom = c * c + d * d;
    float x = scalbnf((a * c + b * d) / denom, -ilogbw);
    float y = scalbnf((b * c - a * d) / denom, -ilogbw);
    if (isnan(x) && isnan(y)) {
        if (denom == 0.0f && (!isnan(a) || !isnan(b))) {
            x = copysignf(INFINITY, c) * a;
            y = copysignf(INFINITY, c) * b;
        } else if ((isinf(a) || isinf(b)) && isfinite(c) && isfinite(d)) {
            a = copysignf(isinf(a) ? 1.0f : 0.0f, a);
            b = copysignf(isinf(b) ? 1.0f : 0.0f, b);
            x = INFINITY * (a * c + b * d);
            y = INFINITY * (b * c - a * d);
        } else if (isinf(logbw) && logbw > 0.0f && isfinite(a) && isfinite(b)) {
            c = copysignf(isinf(c) ? 1.0f : 0.0f, c);
            d = copysignf(isinf(d) ? 1.0f : 0.0f, d);
            x = 0.0f * (a * c + b * d);
            y = 0.0f * (b * c - a * d);
        }
    }
    return __builtin_complex(x, y);
}

// The adjugate (cofactors with fused multiply-subtract, as Apple's libm) times the reciprocal of the determinant
simd_double3x3 __invert_d3(simd_double3x3 m) {
    const double a = m.columns[0].x, b = m.columns[0].y, c = m.columns[0].z;
    const double d = m.columns[1].x, e = m.columns[1].y, f = m.columns[1].z;
    const double g = m.columns[2].x, h = m.columns[2].y, i = m.columns[2].z;
    const double c00 = fma(e, i, -(h * f)), c01 = -fma(b, i, -(h * c)), c02 = fma(b, f, -(c * e));
    const double c10 = -fma(d, i, -(f * g)), c11 = fma(a, i, -(c * g)), c12 = -fma(a, f, -(c * d));
    const double c20 = fma(d, h, -(e * g)), c21 = -fma(a, h, -(b * g)), c22 = fma(a, e, -(b * d));
    const double det = fma(c02, g, fma(c00, a, -(fma(b, i, -(h * c)) * d)));
    const double r = 1.0 / det;
    simd_double3x3 out;
    out.columns[0] = (simd_double3){c00 * r, c01 * r, c02 * r};
    out.columns[1] = (simd_double3){c10 * r, c11 * r, c12 * r};
    out.columns[2] = (simd_double3){c20 * r, c21 * r, c22 * r};
    return out;
}

// No effect on macOS 10.12 and later
void arc4random_stir(void) {}

int timespec_get(struct timespec *ts, int base) {
    if (base != TIME_UTC)
        return 0;
    return clock_gettime(CLOCK_REALTIME, ts) == 0 ? base : 0;
}

long long strtonum(const char *text, long long minimum, long long maximum, const char **error) {
    long long value = 0;
    const char *problem = NULL;
    int code = 0;
    if (minimum > maximum) {
        problem = "invalid";
        code = EINVAL;
    } else {
        char *end;
        errno = 0;
        value = strtoll(text, &end, 10);
        if (text == end || *end != '\0') {
            problem = "invalid";
            code = EINVAL;
        } else if ((value == LLONG_MIN && errno == ERANGE) || value < minimum) {
            problem = "too small";
            code = ERANGE;
        } else if ((value == LLONG_MAX && errno == ERANGE) || value > maximum) {
            problem = "too large";
            code = ERANGE;
        }
    }
    if (error)
        *error = problem;
    errno = code;
    return problem ? 0 : value;
}

// The mask is unset (0) until it is set; the default filter is LOG_UPTO(LOG_NOTICE)
static int logMask;
int setlogmask(int mask) {
    int old = logMask;
    if (mask) {
        if (!old)
            old = 0x3f;
        logMask = mask;
    }
    return old;
}

static int processors(void) {
    const long n = sysconf(_SC_NPROCESSORS_ONLN);
    return n > 0 ? (int)n : 1;
}

int pthread_qos_max_parallelism(unsigned qos, uint64_t flags) {
    switch (qos) {
    case 0x00: case 0x05: case 0x09: case 0x11: case 0x15: case 0x19: case 0x21:
        break;
    default:
        errno = EINVAL;
        return -1;
    }
    if (flags & ~(uint64_t)3) {
        errno = EINVAL;
        return -1;
    }
    return processors();
}

int pthread_time_constraint_max_parallelism(uint64_t flags) {
    if (flags & ~(uint64_t)3) {
        errno = EINVAL;
        return -1;
    }
    return processors();
}

typedef void (*pthread_introspection_hook_t)(unsigned event, pthread_t thread, void *address, size_t size);
static pthread_introspection_hook_t introspectionHook;
pthread_introspection_hook_t pthread_introspection_hook_install(pthread_introspection_hook_t hook) {
    pthread_introspection_hook_t old = introspectionHook;
    introspectionHook = hook;
    return old;
}

// Mach scheduling policies: timesharing, round robin and FIFO are accepted
int thread_policy(unsigned thread, int policy, void *base, unsigned count, int setLimit) {
    (void)thread; (void)base; (void)count; (void)setLimit;
    return (policy == 1 || policy == 2 || policy == 4) ? 0 : 28;   // KERN_INVALID_POLICY
}

// Catalogue search and object id lookup: not supported by the volume
int searchfs(const char *path, struct fssearchblock *searchBlock, unsigned long *matches, unsigned int scriptCode, unsigned int options,
             struct searchstate *state) {
    (void)path; (void)searchBlock; (void)scriptCode; (void)options; (void)state;
    if (matches)
        *matches = 0;
    errno = ENOTSUP;
    return -1;
}

ssize_t fsgetpath(char *buffer, size_t size, fsid_t *fsid, uint64_t object) {
    (void)buffer; (void)size; (void)fsid; (void)object;
    errno = ENOTSUP;
    return -1;
}

// removefile(3): REMOVEFILE_RECURSIVE 1, REMOVEFILE_KEEP_PARENT 2
int removefile(const char *path, void *state, uint32_t flags) {
    (void)state;
    struct stat st;
    if (lstat(path, &st) != 0)
        return -1;
    if (!S_ISDIR(st.st_mode))
        return unlink(path);
    if (!(flags & 1)) {
        if (flags & 2)
            return 0;
        return rmdir(path);
    }
    char *const paths[] = {(char *)path, NULL};
    FTS *fts = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR | FTS_XDEV, NULL);
    if (!fts)
        return -1;
    int result = 0;
    for (FTSENT *e; (e = fts_read(fts));) {
        switch (e->fts_info) {
        case FTS_DP:
            if (e->fts_level == 0 && (flags & 2))
                break;
            if (rmdir(e->fts_accpath) != 0)
                result = -1;
            break;
        case FTS_F: case FTS_SL: case FTS_SLNONE: case FTS_DEFAULT:
            if (unlink(e->fts_accpath) != 0)
                result = -1;
            break;
        default:
            break;
        }
    }
    fts_close(fts);
    return result;
}

// Audio workgroups (os/workgroup.h): Linux has no work intervals; joining, leaving and the interval calls
// succeed, a cancelled workgroup cannot be joined.
struct workgroupState { const void *workgroup; int cancelled; };
static struct workgroupState cancelledGroups[64];
static pthread_mutex_t workgroupLock = PTHREAD_MUTEX_INITIALIZER;

static int isCancelled(const void *wg) {
    int found = 0;
    pthread_mutex_lock(&workgroupLock);
    for (int i = 0; i < 64; i++)
        if (cancelledGroups[i].workgroup == wg && cancelledGroups[i].cancelled)
            found = 1;
    pthread_mutex_unlock(&workgroupLock);
    return found;
}

void os_workgroup_cancel(void *wg) {
    pthread_mutex_lock(&workgroupLock);
    for (int i = 0; i < 64; i++) {
        if (!cancelledGroups[i].workgroup || cancelledGroups[i].workgroup == wg) {
            cancelledGroups[i].workgroup = wg;
            cancelledGroups[i].cancelled = 1;
            break;
        }
    }
    pthread_mutex_unlock(&workgroupLock);
}

typedef struct { uint32_t sig; char opaque[36]; } workgroupJoinToken;

int os_workgroup_join(void *wg, workgroupJoinToken *token) {
    if (!wg || isCancelled(wg))
        return EINVAL;
    if (token)
        token->sig = 0x4a4f494e;
    return 0;
}

void os_workgroup_leave(void *wg, workgroupJoinToken *token) {
    (void)wg;
    if (token)
        token->sig = 0;
}

int os_workgroup_max_parallel_threads(void *wg, void *attributes) {
    (void)wg; (void)attributes;
    return processors();
}

int os_workgroup_interval_start(void *wg, uint64_t start, uint64_t deadline, void *data) {
    (void)data;
    if (!wg || deadline < start || isCancelled(wg))
        return EINVAL;
    return 0;
}

int os_workgroup_interval_update(void *wg, uint64_t deadline, void *data) {
    (void)deadline; (void)data;
    return wg && !isCancelled(wg) ? 0 : EINVAL;
}

int os_workgroup_interval_finish(void *wg, void *data) {
    (void)data;
    return wg && !isCancelled(wg) ? 0 : EINVAL;
}
