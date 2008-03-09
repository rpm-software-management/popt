#include "system.h"
#include <stdarg.h>
#include "poptint.h"

/*@-varuse +charint +ignoresigns @*/
/*@unchecked@*/ /*@observer@*/
static const unsigned char utf8_skip_data[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};
/*@=varuse =charint =ignoresigns @*/

#if defined(HAVE_DCGETTEXT) && !defined(__LCLINT__)
/*
 * Rebind a "UTF-8" codeset for popt's internal use.
 */
char *
POPT_dgettext(const char * dom, const char * str)
{
    char * codeset = NULL;
    char * retval = NULL;

    if (!dom) 
	dom = textdomain(NULL);
    codeset = bind_textdomain_codeset(dom, NULL);
    bind_textdomain_codeset(dom, "UTF-8");
    retval = dgettext(dom, str);
    bind_textdomain_codeset(dom, codeset);

    return retval;
}
#endif

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

    if (codeset != NULL && strcmp(codeset, "UTF-8") != 0
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
	while (done == 0 && is_error == 0) {
	    err = iconv(fd, &pin, &ib, &pout, &ob);

	    if (err == (size_t)-1) {
		switch (errno) {
		case EINVAL:
		    done = 1;
		    /*@switchbreak@*/ break;
		case E2BIG:
		{   size_t used = (size_t)(pout - dest_str);
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

const char *
POPT_prev_char (const char *str)
{
    const char *p = str;

    while (1) {
	p--;
	if (((unsigned)*p & 0xc0) != (unsigned)0x80)
	    return p;
    }
}

const char *
POPT_next_char (const char *str)
{
    const char *p = str;

    while (*p != '\0') {
	p++;
	if (((unsigned)*p & 0xc0) != (unsigned)0x80)
	    break;
    }
    return p;
}

int
POPT_fprintf (FILE * stream, const char * format, ...)
{
    char * b = NULL, * ob = NULL;
    size_t nb = (size_t)1;
    int rc;
    va_list ap;

    va_start(ap, format);
    while ((b = realloc(b, nb)) != NULL) {
	rc = vsnprintf(b, nb, format, ap);
	if (rc > -1) {	/* glibc 2.1 */
	    if ((size_t)rc < nb)
		break;
	    nb = (size_t)(rc + 1);	/* precise buffer length known */
	} else 		/* glibc 2.0 */
	    nb += (nb < (size_t)100 ? (size_t)100 : nb);
	ob = b;
    }
    va_end(ap);

    rc = 0;
    if (b != NULL) {
#ifdef HAVE_ICONV
	ob = strdup_locale_from_utf8(b);
	if (ob != NULL) {
	    rc = fprintf(stream, "%s", ob);
	    free(ob);
	} else
#endif
	    rc = fprintf(stream, "%s", b);
	free (b);
    } else if (ob != NULL)
	free(ob);

    return rc;
}
