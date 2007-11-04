#include "system.h"
#include <stdarg.h>
#include "poptint.h"

#ifdef HAVE_ICONV
static /*@only@*/ /*@null@*/ char *
strdup_locale_from_utf8 (/*@null@*/ char *buffer)
	/*@*/
{
    char *codeset = NULL;
    char *dest_str;
    iconv_t fd;

    if (buffer == NULL)
	return NULL;

#ifdef HAVE_LANGINFO_H
/*@-type@*/
    codeset = nl_langinfo (CODESET);
/*@=type@*/
#endif

    if (codeset && strcmp(codeset, "UTF-8")
     && (fd = iconv_open(codeset, "UTF-8")) != (iconv_t)-1)
    {
	char *pin = buffer;
	char *pout = NULL;
	size_t ib, ob, dest_size;
	int done;
	int is_error;
	size_t err;
	char *shift_pin = NULL;
	int xx;

	err = iconv(fd, NULL, &ib, &pout, &ob);
	dest_size = ob = ib = strlen(buffer);
	dest_str = pout = malloc((dest_size + 1) * sizeof(*dest_str));
	if (dest_str)
	    *dest_str = '\0';
	done = is_error = 0;
	if (pout != NULL)
	while (!done && !is_error) {
	    err = iconv(fd, &pin, &ib, &pout, &ob);

	    if (err == (size_t)-1) {
		switch (errno) {
		case EINVAL:
		    done = 1;
		    /*@switchbreak@*/ break;
		case E2BIG:
		{   size_t used = pout - dest_str;
		    dest_size *= 2;
		    dest_str = realloc(dest_str, (dest_size + 1) * sizeof(*dest_str));
		    if (dest_str == NULL) {
			is_error = 1;
			continue;
		    }
		    pout = dest_str + used;
		    ob = dest_size - used;
		}   /*@switchbreak@*/ break;
		case EILSEQ:
		    is_error = 1;
		    /*@switchbreak@*/ break;
		default:
		    is_error = 1;
		    /*@switchbreak@*/ break;
		}
	    } else {
		if (shift_pin == NULL) {
		    shift_pin = pin;
		    pin = NULL;
		    ib = 0;
		} else {
		    done = 1;
		}
	    }
	}
	xx = iconv_close(fd);
	if (pout)
	    *pout = '\0';
	if (dest_str != NULL)
	    dest_str = xstrdup(dest_str);
    } else {
	dest_str = xstrdup(buffer);
    }

    return dest_str;
}
#endif

/*@-mustmod@*/	/* LCL: can't see the ap modification. */
static /*@only@*/ /*@null@*/ char *
strdup_vprintf (const char *format, va_list ap)
	/*@modifies ap @*/
{
    char *buffer;
    char c;
    va_list apc;
    int xx;

/*@-noeffectuncon -unrecog @*/
    va_copy(apc, ap);	/* XXX linux amd64/ppc needs a copy. */
/*@=noeffectuncon =unrecog @*/

    buffer = calloc(sizeof(*buffer), vsnprintf (&c, 1, format, ap) + 1);
    if (buffer != NULL)
	xx = vsprintf(buffer, format, apc);

    va_end(apc);

    return buffer;
}
/*@=mustmod@*/

char *
POPT_prev_char (const char *str)
{
    char *p = (char *)str;

    while (1) {
	p--;
	if ((*p & 0xc0) != (char)0x80)
	    return (char *)p;
    }
}

int
POPT_fprintf (FILE* stream, const char *format, ...)
{
    int retval = 0;
    va_list args;
    char *buffer = NULL;
#ifdef HAVE_ICONV
    char *locale_str = NULL;
#endif

    va_start (args, format);
    buffer = strdup_vprintf(format, args);
    va_end (args);
    if (buffer == NULL)
	return retval;

#ifdef HAVE_ICONV
    locale_str = strdup_locale_from_utf8(buffer);
    if (locale_str) {
	retval = fprintf(stream, "%s", locale_str);
	free(locale_str);
    } else
#endif
    {
	retval = fprintf(stream, "%s", buffer);
    }
    free (buffer);

    return retval;
}
