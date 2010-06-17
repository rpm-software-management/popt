/** \ingroup popt
 * \file popt/poptparse.c
 */

/* (C) 1998-2002 Red Hat, Inc. -- Licensing details are in the COPYING
   file accompanying popt source distributions, available from 
   ftp://ftp.rpm.org/pub/rpm/dist. */

#include "system.h"

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#else
#define assert(_x)
#endif

#define POPT_ARGV_ARRAY_GROW_DELTA 5

int poptDupArgv(int argc, const char **argv,
		int * argcPtr, const char *** argvPtr)
{
    size_t nb = (argc + 1) * sizeof(*argv);
    const char ** argv2;
    char * dst;
    int i;

    if (argc <= 0 || argv == NULL)	/* XXX can't happen */
	return POPT_ERROR_NOARG;

#if defined(SUPPORT_CONTIGUOUS_ARGV)
    for (i = 0; i < argc; i++) {
	if (argv[i] == NULL)
	    return POPT_ERROR_NOARG;
	nb += strlen(argv[i]) + 1;
    }
#endif
	
    dst = xmalloc(nb);
assert(dst);	/* XXX can't happen */
    if (dst == NULL)
	return POPT_ERROR_MALLOC;
    argv2 = (void *) dst;
#if defined(SUPPORT_CONTIGUOUS_ARGV)
    dst += (argc + 1) * sizeof(*argv);
    *dst = '\0';
#endif

    for (i = 0; i < argc; i++) {
#if defined(SUPPORT_CONTIGUOUS_ARGV)
	argv2[i] = dst;
	dst = stpcpy(dst, argv[i]);
	dst++;	/* trailing NUL */
#else
	argv2[i] = xstrdup(argv[i]);
#endif
    }
    argv2[argc] = NULL;

    if (argvPtr) {
	*argvPtr = argv2;
    } else {
#if !defined(SUPPORT_CONTIGUOUS_ARGV)
	if (argv2)
	for (i = 0; i < argc; i++)
	    if (argv2[i]) free((void *)argv2[i]);	/* XXX _free */
#endif
	free((void *)argv2);	/* XXX _free */
	argv2 = NULL;
    }
    if (argcPtr)
	*argcPtr = argc;
    return 0;
}

int poptParseArgvString(const char * s, int * argcPtr, const char *** argvPtr)
{
    const char * src;
    char quote = '\0';
    int argvAlloced = POPT_ARGV_ARRAY_GROW_DELTA;
    const char ** argv = xmalloc(sizeof(*argv) * argvAlloced);
    int argc = 0;
    size_t buflen = strlen(s) + 1;
    char * buf, * bufOrig = NULL;
    int rc = POPT_ERROR_MALLOC;

assert(argv);	/* XXX can't happen */
    if (argv == NULL) return rc;

    buf = bufOrig = xcalloc((size_t)1, buflen);
assert(buf);	/* XXX can't happen */
    if (buf == NULL) {
	free(argv);		/* XXX _free */
	return rc;
    }
    argv[argc] = buf;

    for (src = s; *src != '\0'; src++) {
	if (quote == *src) {
	    quote = '\0';
	} else if (quote != '\0') {
	    if (*src == '\\') {
		src++;
		if (!*src) {
		    rc = POPT_ERROR_BADQUOTE;
		    goto exit;
		}
		if (*src != quote) *buf++ = '\\';
	    }
	    *buf++ = *src;
	} else if (_isspaceptr(src)) {
	    if (*argv[argc] != '\0') {
		buf++, argc++;
		if (argc == argvAlloced) {
		    argvAlloced += POPT_ARGV_ARRAY_GROW_DELTA;
		    argv = xrealloc(argv, sizeof(*argv) * argvAlloced);
assert(argv);	/* XXX can't happen */
		    if (argv == NULL) goto exit;
		}
		argv[argc] = buf;
	    }
	} else switch (*src) {
	  case '"':
	  case '\'':
	    quote = *src;
	    /*@switchbreak@*/ break;
	  case '\\':
	    src++;
	    if (!*src) {
		rc = POPT_ERROR_BADQUOTE;
		goto exit;
	    }
	    /*@fallthrough@*/
	  default:
	    *buf++ = *src;
	    /*@switchbreak@*/ break;
	}
    }

    if (strlen(argv[argc])) {
	argc++, buf++;
    }

    rc = poptDupArgv(argc, argv, argcPtr, argvPtr);

exit:
    if (bufOrig) free(bufOrig);		/* XXX _free */
    if (argv) free(argv);		/* XXX _free */
    return rc;
}

/* still in the dev stage.
 * return values, perhaps 1== file erro
 * 2== line to long
 * 3== umm.... more?
 */
int poptConfigFileToString(FILE *fp, char ** argstrp,
		/*@unused@*/ UNUSED(int flags))
{
    char line[8192];	/* XXX configurable? */
    char * argstr;
    char * p;
    char * q;
    char * x;
    size_t t;
    size_t argvlen = 0;
    size_t maxlinelen = sizeof(line);
    size_t linelen;
    size_t maxargvlen = (size_t)480;

    if (argstrp)
	*argstrp = NULL;

    /*   |   this_is   =   our_line
     *	     p             q      x
     */

    if (fp == NULL)
	return POPT_ERROR_NULLARG;

    argstr = xmalloc(maxargvlen * sizeof(*argstr));
assert(argstr);	/* XXX can't happen */
    if (argstr == NULL) return POPT_ERROR_MALLOC;
    argstr[0] = '\0';

    while (fgets(line, (int)maxlinelen, fp) != NULL) {
	p = line;

	/* loop until first non-space char or EOL */
	while( *p != '\0' && _isspaceptr(p) )
	    p++;

	linelen = strlen(p);
	if (linelen >= maxlinelen-1) {
	    free(argstr);	/* XXX _free */
	    return POPT_ERROR_OVERFLOW;	/* XXX line too long */
	}

	if (*p == '\0' || *p == '\n') continue;	/* line is empty */
	if (*p == '#') continue;		/* comment line */

	q = p;

	while (*q != '\0' && (!_isspaceptr(q)) && *q != '=')
	    q++;

	if (_isspaceptr(q)) {
	    /* a space after the name, find next non space */
	    *q++='\0';
	    while( *q != '\0' && _isspaceptr(q) ) q++;
	}
	if (*q == '\0') {
	    /* single command line option (ie, no name=val, just name) */
	    q[-1] = '\0';		/* kill off newline from fgets() call */
	    argvlen += (t = (size_t)(q - p)) + (sizeof(" --")-1);
	    if (argvlen >= maxargvlen) {
		maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
		argstr = xrealloc(argstr, maxargvlen);
assert(argstr);	/* XXX can't happen */
		if (argstr == NULL) return POPT_ERROR_MALLOC;
	    }
	    strcat(argstr, " --");	/* XXX stpcpy */
	    strcat(argstr, p);		/* XXX stpcpy */
	    continue;
	}
	if (*q != '=')
	    continue;	/* XXX for now, silently ignore bogus line */
		
	/* *q is an equal sign. */
	*q++ = '\0';

	/* find next non-space letter of value */
	while (*q != '\0' && _isspaceptr(q))
	    q++;
	if (*q == '\0')
	    continue;	/* XXX silently ignore missing value */

	/* now, loop and strip all ending whitespace */
	x = p + linelen;
	while (_isspaceptr(--x))
	    *x = '\0';	/* null out last char if space (including fgets() NL) */

	/* rest of line accept */
	t = (size_t)(x - p);
	argvlen += t + (sizeof("' --='")-1);
	if (argvlen >= maxargvlen) {
	    maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
	    argstr = xrealloc(argstr, maxargvlen);
assert(argstr);	/* XXX can't happen */
	    if (argstr == NULL) return POPT_ERROR_MALLOC;
	}
	strcat(argstr, " --");	/* XXX stpcpy */
	strcat(argstr, p);	/* XXX stpcpy */
	strcat(argstr, "=\"");	/* XXX stpcpy */
	strcat(argstr, q);	/* XXX stpcpy */
	strcat(argstr, "\"");	/* XXX stpcpy */
    }

    *argstrp = argstr;
    return 0;
}
