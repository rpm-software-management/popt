/** \ingroup popt
 * \file popt/poptparse.c
 */

/* (C) 1998-2002 Red Hat, Inc. -- Licensing details are in the COPYING
   file accompanying popt source distributions, available from 
   ftp://ftp.rpm.org/pub/rpm/dist. */

#include "system.h"

#include <poptint.h>

#if defined(HAVE_ASSERT_H)
#include <assert.h>
#else
#define assert(_x)
#endif

static const char ** poptArgvFree(/*@only@*/ const char ** av)
{
#if !defined(SUPPORT_CONTIGUOUS_ARGV)
    if (av) {
    int i;
    for (i = 0; av[i]; i++)
	av[i] = _free(av[i]);
    }
#endif
    av = _free(av);
    return NULL;
}

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

    if (argvPtr)
	*argvPtr = argv2;
    else
	argv2 = poptArgvFree(argv2);
    if (argcPtr)
	*argcPtr = argc;
    return 0;
}

int poptParseArgvString(const char * s, int * argcPtr, const char *** argvPtr)
{
    const char * se;
    char quote = '\0';
    size_t argvAlloced = POPT_ARGV_ARRAY_GROW_DELTA;
    const char ** argv = xmalloc(sizeof(*argv) * argvAlloced);
    int argc = 0;
    size_t ns = strlen(s);
    char * t = NULL;
    char * te;
    int rc = POPT_ERROR_MALLOC;

assert(argv);	/* XXX can't happen */
    if (argv == NULL) return rc;

    te = t = xcalloc((size_t)1, ns + 1);
assert(te);	/* XXX can't happen */
    if (te == NULL) {
	argv = _free(argv);
	return rc;
    }
    argv[argc] = te;

    for (se = s; *se != '\0'; se++) {
	if (quote == *se) {
	    quote = '\0';
	} else if (quote != '\0') {
	    if (*se == '\\') {
		se++;
		if (*se == '\0') {
		    rc = POPT_ERROR_BADQUOTE;
		    goto exit;
		}
		if (*se != quote) *te++ = '\\';
	    }
	    *te++ = *se;
	} else if (_isspaceptr(se)) {
	    if (*argv[argc] != '\0') {
		te++, argc++;
		if (argc == argvAlloced) {
		    argvAlloced += POPT_ARGV_ARRAY_GROW_DELTA;
		    argv = xrealloc(argv, sizeof(*argv) * argvAlloced);
assert(argv);	/* XXX can't happen */
		    if (argv == NULL) goto exit;
		}
		argv[argc] = te;
	    }
	} else
	switch (*se) {
	  case '"':
	  case '\'':
	    quote = *se;
	    /*@switchbreak@*/ break;
	  case '\\':
	    se++;
	    if (*se == '\0') {
		rc = POPT_ERROR_BADQUOTE;
		goto exit;
	    }
	    /*@fallthrough@*/
	  default:
	    *te++ = *se;
	    /*@switchbreak@*/ break;
	}
    }

    if (strlen(argv[argc])) {
	argc++, te++;
    }

    rc = poptDupArgv(argc, argv, argcPtr, argvPtr);

exit:
    t = _free(t);
    argv = _free(argv);
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
    size_t nline = 8192;	/* XXX configurable? */
    char * line = alloca(nline);
    char * argstr;
    char * q;
    char * x;
    size_t t;
    size_t argvlen = 0;
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

    while (fgets(line, (int)nline, fp) != NULL) {
	char * l = line;
	size_t nl;

	/* loop until first non-space char or EOL */
	while( *l != '\0' && _isspaceptr(l) )
	    l++;

	nl = strlen(l);
	if (nl >= nline-1) {
	    argstr = _free(argstr);
	    return POPT_ERROR_OVERFLOW;	/* XXX line too long */
	}

	if (*l == '\0' || *l == '\n') continue;	/* line is empty */
	if (*l == '#') continue;		/* comment line */

	q = l;

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
	    argvlen += (t = (size_t)(q - l)) + (sizeof(" --")-1);
	    if (argvlen >= maxargvlen) {
		maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
		argstr = xrealloc(argstr, maxargvlen);
assert(argstr);	/* XXX can't happen */
		if (argstr == NULL) return POPT_ERROR_MALLOC;
	    }
	    strcat(argstr, " --");	/* XXX stpcpy */
	    strcat(argstr, l);		/* XXX stpcpy */
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
	x = l + nl;
	while (_isspaceptr(--x))
	    *x = '\0';	/* null out last char if space (including fgets() NL) */

	/* rest of line accept */
	t = (size_t)(x - l);
	argvlen += t + (sizeof("' --='")-1);
	if (argvlen >= maxargvlen) {
	    maxargvlen = (t > maxargvlen) ? t*2 : maxargvlen*2;
	    argstr = xrealloc(argstr, maxargvlen);
assert(argstr);	/* XXX can't happen */
	    if (argstr == NULL) return POPT_ERROR_MALLOC;
	}
	strcat(argstr, " --");	/* XXX stpcpy */
	strcat(argstr, l);	/* XXX stpcpy */
	strcat(argstr, "=\"");	/* XXX stpcpy */
	strcat(argstr, q);	/* XXX stpcpy */
	strcat(argstr, "\"");	/* XXX stpcpy */
    }

    *argstrp = argstr;
    return 0;
}
