#if !defined(CKDINT_H)
#define CKDINT_H

#include <limits.h>
#include <stdint.h>

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

#if (CKDINT_HAS_BUILTIN(__builtin_add_overflow_p) && \
     CKDINT_HAS_BUILTIN(__builtin_sub_overflow_p) && \
     CKDINT_HAS_BUILTIN(__builtin_mul_overflow_p))

#define ckdint_add_test(a, b, expr) \
	__builtin_add_overflow_p((a), (b), (expr))

#define ckjdint_sub_test(a, b, expr) \
	__builtin_add_overflow_p((a), (b), (expr))

#define ckdint_mul_test(a, b, expr) \
	__builtin_mul_overflow_p((a), (b), (expr))

#else /* CKDINT_HAS_BUILTIN(__builtin_{add,sub,mul}_overflow_p) */

#define CKDINT_IS_SIGNED(expr)					\
	_Generic((expr),					\
		char:			(CHAR_MIN < 0),		\
		signed char:		1,			\
		short:			1,			\
		int:			1,			\
		long:			1,			\
		unsigned char:		0,			\
		unsigned short:		0,			\
		unsigned int:		0,			\
		unsigned long:		0			\
	)

#define CKDINT_TEST_OP(op, a, b, expr)				\
	_Generic((expr),					\
		char:						\
			op((a), (b), CHAR_MIN, CHAR_MAX),	\
		signed char:					\
			op((a), (b), SCHAR_MIN, SCHAR_MAX),	\
		short:						\
			op((a), (b), SHRT_MIN, SHRT_MAX),	\
		int:						\
			op((a), (b), INT_MIN, INT_MAX),		\
		long:						\
			op((a), (b), LONG_MIN, LONG_MAX),	\
		unsigned char:					\
			op((a), (b), 0, UCHAR_MAX),		\
		unsigned short:					\
			op((a), (b), 0, USHRT_MAX),		\
		unsigned int:					\
			op((a), (b), 0, UINT_MAX),		\
		unsigned long:					\
			op((a), (b), 0, ULONG_MAX)		\
	)

#if CKDINT_HAS_BUILTIN(__builtin_add_overflow_p)

#define ckdint_add_test(a, b, expr) \
	__builtin_add_overflow_p((a), (b), (expr))

#else /* CKDINT_HAS_BUILTIN(__builtin_add_overflow_p) */

#define ckdint_add_test(a, b, expr) \
	CKDINT_TEST_OP(CKDINT_TEST_ADD, (a), (b), (expr))

#define CKDINT_TEST_ADD(a, b, min, max)				\
	(((min) < 0)						\
		? CKDINT_TEST_ADD_SRES((a), (b), (min), (max))	\
		: CKDINT_TEST_ADD_URES((a), (b), (max)))

#define CKDINT_TEST_ADD_SRES(a, b, min, max)			\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_sadd_sres(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_mixadd_sres(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixadd_sres(		\
				(intmax_t)(b),			\
				(uintmax_t)(a),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_uadd(			\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
	)

#define CKDINT_TEST_ADD_URES(a, b, max)				\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_sadd_ures(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_mixadd_ures(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixadd_ures(		\
				(intmax_t)(b),			\
				(uintmax_t)(a),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_uadd(			\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
	)

#endif /* CKDINT_HAS_BUILTIN(__builtin_add_overflow_p) */

#endif /* CKDINT_HAS_BUILTIN(__builtin_{add,sub,mul}_overflow_p) */

#if CKDINT_HAS_INCLUDE(<stdckdint.h>)

#include <stdckdint.h>

#define ckdint_add(res, a, b)	ckd_add((res), (a), (b))
#define ckdint_sub(res, a, b)	ckd_sub((res), (a), (b))
#define ckdint_mul(res, a, b)	ckd_mul((res), (a), (b))

#else /* CKDINT_HAS_INCLUDE(<stdckdint.h>) */

#if CKDINT_HAS_BUILTIN(__builtin_add_overflow)
#define ckdint_add(res, a, b)	__builtin_add_overflow((a), (b), (res))
#endif

#if CKDINT_HAS_BUILTIN(__builtin_sub_overflow)
#define ckdint_sub(res, a, b)	__builtin_sub_overflow((a), (b), (res))
#endif

#if CKDINT_HAS_BUILTIN(__builtin_mul_overflow)
#define ckdint_mul(res, a, b)	__builtin_mul_overflow((a), (b), (res))
#endif

#endif /* CKDINT_HAS_INCLUDE(<stdckdint.h>) */

#endif /* !defined(CKDINT_H) */
