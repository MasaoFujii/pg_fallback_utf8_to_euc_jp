/*-------------------------------------------------------------------------
 *
 * pg_fallback_utf8_to_euc_jp.c
 *   provides an encoding conversion from UTF-8 to EUC_JP
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "conv/fallback_utf8_to_euc_jp.extra"
#include "conv/fallback_utf8_to_euc_jp.map"
#include "funcapi.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(pg_fallback_utf8_to_euc_jp);

/*
 * The function prototypes are created as a part of PG_FUNCTION_INFO_V1
 * macro since 9.4, and hence the declaration of the function prototypes
 * here is necessary only for 9.3 or before.
 */
#if PG_VERSION_NUM < 90400
Datum pg_fallback_utf8_to_euc_jp(PG_FUNCTION_ARGS);
#endif

/*
 * Comparison routine to bsearch() for UTF-8 -> local code.
 */
static int
compare_utf_to_local(const void *p1, const void *p2)
{
	uint32		v1,
				v2;

	v1 = *(const uint32 *) p1;
	v2 = ((const pg_utf_to_local *) p2)->utf;
	return (v1 > v2) ? 1 : ((v1 == v2) ? 0 : -1);
}

/*
 * Perform extra mapping of UTF-8 ranges to EUC_JP.
 */
static uint32
extra_fallback_utf8_to_euc_jp(uint32 utf)
{
	const pg_utf_to_local *p;

	p = bsearch(&utf, ExtraULmapEUC_JP, lengthof(ExtraULmapEUC_JP),
				sizeof(pg_utf_to_local), compare_utf_to_local);
	return p ? p->code : 0x3f;
}

#if PG_VERSION_NUM < 90500
/*
 * callback function for algorithmic encoding conversions (in either direction)
 *
 * if function returns zero, it does not know how to convert the code
 */
typedef uint32 (*utf_local_conversion_func) (uint32 code);

/*
 * store 32bit character representation into multibyte stream
 */
static inline unsigned char *
pg_store_coded_char(unsigned char *dest, uint32 code)
{
	if (code & 0xff000000)
		*dest++ = code >> 24;
	if (code & 0x00ff0000)
		*dest++ = code >> 16;
	if (code & 0x0000ff00)
		*dest++ = code >> 8;
	if (code & 0x000000ff)
		*dest++ = code;
	return dest;
}

static void
PgSimpleUtfToLocal(const unsigned char *utf, int len,
		   unsigned char *iso,
		   const pg_utf_to_local *map, int mapsize,
		   utf_local_conversion_func conv_func,
		   int encoding)
{
	uint32		iutf;
	int			l;
	const pg_utf_to_local *p;

	if (!PG_VALID_ENCODING(encoding))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid encoding number: %d", encoding)));

	for (; len > 0; len -= l)
	{
		/* "break" cases all represent errors */
		if (*utf == '\0')
			break;

		l = pg_utf_mblen(utf);
		if (len < l)
			break;

		if (!pg_utf8_islegal(utf, l))
			break;

		if (l == 1)
		{
			/* ASCII case is easy, assume it's one-to-one conversion */
			*iso++ = *utf++;
			continue;
		}

		/* collect coded char of length l */
		if (l == 2)
		{
			iutf = *utf++ << 8;
			iutf |= *utf++;
		}
		else if (l == 3)
		{
			iutf = *utf++ << 16;
			iutf |= *utf++ << 8;
			iutf |= *utf++;
		}
		else if (l == 4)
		{
			iutf = *utf++ << 24;
			iutf |= *utf++ << 16;
			iutf |= *utf++ << 8;
			iutf |= *utf++;
		}
		else
		{
			elog(ERROR, "unsupported character length %d", l);
			iutf = 0;			/* keep compiler quiet */
		}

		/* Now check ordinary map */
		p = bsearch(&iutf, map, mapsize,
					sizeof(pg_utf_to_local), compare_utf_to_local);

		if (p)
		{
			iso = pg_store_coded_char(iso, p->code);
			continue;
		}

		/* if there's a conversion function, try that */
		if (conv_func)
		{
			uint32		converted = (*conv_func) (iutf);

			if (converted)
			{
				iso = pg_store_coded_char(iso, converted);
				continue;
			}
		}

		/* failed to translate this character */
		report_untranslatable_char(PG_UTF8, encoding,
								   (const char *) (utf - l), len);
	}

	/* if we broke out of loop early, must be invalid input */
	if (len > 0)
		report_invalid_encoding(PG_UTF8, (const char *) utf, len);

	*iso = '\0';
}
#endif

/*
 * Convert string from UTF-8 to EUC_JP.
 */
Datum
pg_fallback_utf8_to_euc_jp(PG_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) PG_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) PG_GETARG_CSTRING(3);
	int			len = PG_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(PG_UTF8, PG_EUC_JP);

#if PG_VERSION_NUM >= 90500
	UtfToLocal(src, len, dest,
			   ULmapEUC_JP, lengthof(ULmapEUC_JP),
			   NULL, 0,
			   extra_fallback_utf8_to_euc_jp,
			   PG_EUC_JP);
#else
	PgSimpleUtfToLocal(src, len, dest,
			   ULmapEUC_JP, lengthof(ULmapEUC_JP),
			   extra_fallback_utf8_to_euc_jp,
			   PG_EUC_JP);
#endif

	PG_RETURN_VOID();
}
