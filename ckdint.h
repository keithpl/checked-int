#if !defined(CKDINT_H)
#define CKDINT_H

#include <limits.h>

#if defined(__has_attribute)
#define CKDINT_HAS_ATTRIBUTE	__has_attribute
#else
#define CKDINT_HAS_ATTRIBUTE(x)	0
#endif

#if defined(__has_builtin)
#define CKDINT_HAS_BUILTIN	__has_builtin
#else
#define CKDINT_HAS_BUILTIN(x)	0
#endif

#if defined(__has_include)
#define CKDINT_HAS_INCLUDE	__has_include
#else
#define CKDINT_HAS_INCLUDE(x)	0
#endif

#if CKDINT_HAS_ATTRIBUTE(__always_inline__)
#define CKDINT_ALWAYS_INLINE	__attribute__((__always_inline__)) inline
#else
#define CKDINT_ALWAYS_INLINE	inline
#endif

#if CKDINT_HAS_ATTRIBUTE(__const__)
#define CKDINT_CONST		__attribute__((__const__))
#else
#define CKDINT_CONST
#endif

#define CKDINT_INLINE		CKDINT_ALWAYS_INLINE CKDINT_CONST

#if CKDINT_HAS_INCLUDE(<stdckdint.h>)

#include <stdckdint.h>

#define ckdint_add(res, x, y)	ckd_add((res), (x), (y))
#define ckdint_sub(res, x, y)	ckd_sub((res), (x), (y))
#define ckdint_mul(res, x, y)	ckd_mul((res), (x), (y))

#else /* CKDINT_HAS_INCLUDE(<stdckdint.h>) */

#if (CKDINT_HAS_BUILTIN(__builtin_add_overflow) && \
     CKDINT_HAS_BUILTIN(__builtin_sub_overflow) && \
     CKDINT_HAS_BUILTIN(__builtin_mul_overflow))

#define ckdint_add(res, x, y)	__builtin_add_overflow((x), (y), (res))
#define ckdint_sub(res, x, y)	__builtin_sub_overflow((x), (y), (res))
#define ckdint_mul(res, x, y)	__builtin_mul_overflow((x), (y), (res))

#else /* CKDINT_HAS_BUILTIN(__builtin_{add,sub,mul}_overflow) */

#endif /* CKDINT_HAS_BUILTIN(__builtin_{add,sub,mul}_overflow) */

#endif /* CKDINT_HAS_INCLUDE(<stdckdint.h>) */

#endif /* !defined(CKDINT_H) */
