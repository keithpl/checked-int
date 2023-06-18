#if !defined(CHECKED_INT_H)
#define CHECKED_INT_H

#include <limits.h>

#if defined(__has_attribute)
#define CKDINT_HAS_ATTRIBUTE	__has_attribute
#else
#define CKDINT_HAS_ATTRIBUTE(x)	0
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

#endif /* !defined(CHECKED_INT_H) */
