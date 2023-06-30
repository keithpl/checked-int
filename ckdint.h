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

#define ckdint_test_add(a, b, expr) \
	__builtin_add_overflow_p((a), (b), (expr))

#define ckdint_test_sub(a, b, expr) \
	__builtin_sub_overflow_p((a), (b), (expr))

#define ckdint_test_mul(a, b, expr) \
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

#define CKDINT_SAFE_UNEGATE(x)					\
	((INTMAX_MIN < -INTMAX_MAX)				\
		? (((x) == INTMAX_MIN)				\
			? ((uintmax_t)INTMAX_MAX + 1)		\
			: ((uintmax_t)-(x)))			\
		: ((uintmax_t)-(x)))

#if CKDINT_HAS_BUILTIN(__builtin_add_overflow_p)

#define ckdint_test_add(a, b, expr) \
	__builtin_add_overflow_p((a), (b), (expr))

#else /* CKDINT_HAS_BUILTIN(__builtin_add_overflow_p) */

#define ckdint_test_add(a, b, expr) \
	CKDINT_TEST_OP(CKDINT_TEST_OP_ADD, (a), (b), (expr))

#define CKDINT_TEST_OP_ADD(a, b, min, max)			\
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

static CKDINT_INLINE int ckdint_test_uadd(uintmax_t a, uintmax_t b,
					  uintmax_t max)
{
	return (b > max) || (a > (max - b));
}

static CKDINT_INLINE int ckdint_test_sadd_sres(intmax_t a, intmax_t b,
					       intmax_t min, intmax_t max)
{
	if (b < 0)
		return (a < (min - b)) || ((a + b) > max);

	if (a < 0)
		return (b < (min - a)) || ((a + b) > max);

	return ckdint_test_uadd((uintmax_t)a, (uintmax_t)b, (uintmax_t)max);
}

static CKDINT_INLINE int ckdint_test_mixadd_common(intmax_t a, uintmax_t b,
						   uintmax_t max)
{
	uintmax_t diff;
	uintmax_t min_abs;

	if (b <= max)
		return 0;

	diff = b - max;
	min_abs = CKDINT_SAFE_UNEGATE(INTMAX_MIN);
	return (diff > min_abs) || (a > (intmax_t)-diff);
}

static CKDINT_INLINE int ckdint_test_mixadd_sres(intmax_t a, uintmax_t b,
						 intmax_t min, intmax_t max)
{
	if (a >= 0)
		return ckdint_test_uadd((uintmax_t)a, b, (uintmax_t)max);

	if (a < min) {
		if (b < (uintmax_t)(min - a))
			return 1;
	}

	return ckdint_test_mixadd_common(a, b, (uintmax_t)max);
}

static CKDINT_INLINE int ckdint_test_sadd_ures(intmax_t a, intmax_t b,
					       uintmax_t max)
{
	if (b < 0) {
		if (INTMAX_MIN < -INTMAX_MAX) {
			if ((b == INTMAX_MIN) || (a == INTMAX_MIN))
				return 1;
		}

		if (a < -b)
			return 1;

		return ((uintmax_t)(a + b) > max);
	}

	if (a < 0) {
		if ((uintmax_t)b < CKDINT_SAFE_UNEGATE(a))
			return 1;

		return ((uintmax_t)(a + b) > max);
	}
	return ckdint_test_uadd((uintmax_t)a, (uintmax_t)b, max);
}

static CKDINT_INLINE int ckdint_test_mixadd_ures(intmax_t a, uintmax_t b,
						 uintmax_t max)
{
	if (a >= 0)
		return ckdint_test_uadd((uintmax_t)a, b, max);

	if (b < CKDINT_SAFE_UNEGATE(a))
		return 1;

	return ckdint_test_mixadd_common(a, b, max);
}

#endif /* CKDINT_HAS_BUILTIN(__builtin_add_overflow_p) */

#if CKDINT_HAS_BUILTIN(__builtin_sub_overflow_p)

#define ckdint_test_sub(a, b, expr) \
	__builtin_sub_overflow_p((a), (b), (expr))

#else /* CKDINT_HAS_BUILTIN(__builtin_sub_overflow_p) */

#define ckdint_test_sub(a, b, expr) \
	CKDINT_TEST_OP(CKDINT_TEST_OP_SUB, (a), (b), (expr))

#define CKDINT_TEST_OP_SUB(a, b, min, max)			\
	(((min) < 0)						\
		? CKDINT_TEST_SUB_SRES((a), (b), (min), (max))	\
		: CKDINT_TEST_SUB_URES((a), (b), (max)))

#define CKDINT_TEST_SUB_SRES(a, b, min, max)			\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_ssub_sres(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_mixsub_saub_sres(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixsub_uasb_sres(		\
				(uintmax_t)(a),			\
				(intmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_usub_sres(		\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(intmax_t)(max)			\
			)					\
		)						\
	)

#define CKDINT_TEST_SUB_URES(a, b, max)				\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_ssub_ures(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_mixsub_saub_ures(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixsub_uasb_ures(		\
				(uintmax_t)(a),			\
				(intmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_usub_ures(		\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
	)

#define ckdint_test_sub_same_sign_sres(a, b, max)		\
	(((a) < (b))						\
		? ((INTMAX_MIN < -INTMAX_MAX)			\
			? (((b) - (a) - 1) > (max))		\
			: (((b) - (a)) > (max)))		\
		: (((a) - (b)) > (max)))

static CKDINT_INLINE int ckdint_test_usub_ures(uintmax_t a, uintmax_t b,
					       uintmax_t max)
{
	return (a < b) || ((a - b) > max);
}

static CKDINT_INLINE int ckdint_test_usub_sres(uintmax_t a, uintmax_t b,
					       intmax_t max)
{
	return ckdint_test_sub_same_sign_sres(a, b, (uintmax_t)max);
}

static CKDINT_INLINE int ckdint_test_ssub_sres(intmax_t a, intmax_t b,
					       intmax_t min, intmax_t max)
{
	if (a < 0) {
		if (b >= 0)
			return b > (a - min);

		return ckdint_test_sub_same_sign_sres(a, b, max);
	}

	if (b > 0)
		return ckdint_test_usub_sres((uintmax_t)a, (uintmax_t)b, max);

	return a > (max + b);
}

static CKDINT_INLINE int ckdint_test_mixsub_saub_sres(intmax_t a, uintmax_t b,
						      intmax_t min,
						      intmax_t max)
{
	if (a >= 0)
		return ckdint_test_usub_sres((uintmax_t)a, b, max);

	if ((a - min) < 0)
		return 1;

	return ((uintmax_t)(a - min) < b);
}

static CKDINT_INLINE int ckdint_test_mixsub_uasb_sres(uintmax_t a, intmax_t b,
						      intmax_t min,
						      intmax_t max)
{
	if (b >= 0)
		return ckdint_test_usub_sres(a, (uintmax_t)b, max);

	if ((b - min) < 0)
		return 1;

	if (INTMAX_MIN < -INTMAX_MAX) {
		if ((b == min) && !a)
			return 1;
	}
	return (a > (uintmax_t)(b - min)) || (a > (uintmax_t)(max + b));
}

static CKDINT_INLINE int ckdint_test_ssub_ures(intmax_t a, intmax_t b,
					       uintmax_t max)
{
	if (a < 0) {
		if ((b >= 0) || (a < b))
			return 1;

		return (uintmax_t)(a - b) > max;
	}

	if (b >= 0)
		return ckdint_test_usub_ures((uintmax_t)a, (uintmax_t)b, max);

	if (CKDINT_SAFE_UNEGATE(b) > max)
		return 1;

	return ((uintmax_t)a > (max - CKDINT_SAFE_UNEGATE(b)));
}

static CKDINT_INLINE int ckdint_test_mixsub_saub_ures(intmax_t a, uintmax_t b,
						      uintmax_t max)
{
	if (a < 0)
		return 1;

	if (b > (uintmax_t)a)
		return 1;

	return ((uintmax_t)a - b) > max;
}

static CKDINT_INLINE int ckdint_test_mixsub_uasb_ures(uintmax_t a, intmax_t b,
						      uintmax_t max)
{
	if (b >= 0)
		return ckdint_test_usub_ures(a, (uintmax_t)b, max);

	return (a + CKDINT_SAFE_UNEGATE(b)) > max;
}

#endif /* CKDINT_HAS_BUILTIN(__builtin_sub_overflow_p) */

#if CKDINT_HAS_BUILTIN(__builtin_mul_overflow_p)

#define ckdint_test_mul(a, b, expr) \
	__builtin_mul_overflow_p((a), (b), (expr))

#else /* CKDINT_HAS_BUILTIN(__builtin_mul_overflow_p) */

#define ckdint_test_mul(a, b, expr) \
	CKDINT_TEST_OP(CKDINT_TEST_OP_MUL, (a), (b), (expr))

#define CKDINT_TEST_OP_MUL(a, b, min, max)			\
	(((min) < 0)						\
		? CKDINT_TEST_MUL_SRES((a), (b), (min), (max))	\
		: CKDINT_TEST_MUL_URES((a), (b), (max)))

#define CKDINT_TEST_MUL_SRES(a, b, min, max)			\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_smul_sres(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_mixmul_sres(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixmul_sres(		\
				(intmax_t)(b),			\
				(uintmax_t)(a),			\
				(intmax_t)(min),		\
				(intmax_t)(max)			\
			)					\
			: ckdint_test_umul(			\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
	)

#define CKDINT_TEST_MUL_URES(a, b, max)				\
	(CKDINT_IS_SIGNED(a)					\
		? (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_smul_ures(		\
				(intmax_t)(a),			\
				(intmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_mixmul_ures(		\
				(intmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
		: (CKDINT_IS_SIGNED(b)				\
			? ckdint_test_mixmul_ures(		\
				(intmax_t)(b),			\
				(uintmax_t)(a),			\
				(uintmax_t)(max)		\
			)					\
			: ckdint_test_umul(			\
				(uintmax_t)(a),			\
				(uintmax_t)(b),			\
				(uintmax_t)(max)		\
			)					\
		)						\
	)

static CKDINT_INLINE int ckdint_test_umul(uintmax_t a, uintmax_t b, 
					  uintmax_t max)
{
	return (b > 0) && (a > (max / b));
}

static CKDINT_INLINE int ckdint_test_smul_sres(intmax_t a, intmax_t b,
					       intmax_t min, intmax_t max)
{
	if (!a || !b)
		return 0;

	if (b < 0) {
		if (a < 0)
			return a < (max / b);

		if (b == -1)
			return (uintmax_t)a > CKDINT_SAFE_UNEGATE(min);

		return a > (min / b);
	}

	if (a < 0)
		return a < (min / b);

	return a > (max / b);
}

static CKDINT_INLINE int ckdint_test_mixmul_sres(intmax_t a, uintmax_t b,
						 intmax_t min, intmax_t max)
{
	if (!a || !b)
		return 0;

	if (a < 0)
		return CKDINT_SAFE_UNEGATE(a) > (CKDINT_SAFE_UNEGATE(min) / b);

	return ckdint_test_umul((uintmax_t)a, b, (uintmax_t)max);
}

static CKDINT_INLINE int ckdint_test_smul_ures(intmax_t a, intmax_t b,
					       uintmax_t max)
{
	if (a < 0) {
		if (b > 0)
			return 1;

		if (!b)
			return 0;

		return CKDINT_SAFE_UNEGATE(a) > (max / CKDINT_SAFE_UNEGATE(b));
	}

	if (!a)
		return 0;

	if (b < 0)
		return 1;

	return ckdint_test_umul((uintmax_t)a, (uintmax_t)b, max);
}

static CKDINT_INLINE int ckdint_test_mixmul_ures(intmax_t a, uintmax_t b,
						 uintmax_t max)
{
	if (a >= 0)
		return ckdint_test_umul((uintmax_t)a, b, max);

	return b > 0;
}

#endif /* CKDINT_HAS_BUILTIN(__builtin_mul_overflow_p) */

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
