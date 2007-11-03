/**
 * \file popt/system.h
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined (__GLIBC__) && defined(__LCLINT__)
/*@-declundef@*/
/*@unchecked@*/
extern __const __int32_t *__ctype_tolower;
/*@unchecked@*/
extern __const __int32_t *__ctype_toupper;
/*@=declundef@*/
#endif

#include <ctype.h>

/* XXX isspace(3) has i18n encoding signednesss issues on Solaris. */
#define	_isspaceptr(_chp)	isspace((int)(*(unsigned char *)(_chp)))

#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#ifdef HAVE_MCHECK_H
#include <mcheck.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __NeXT
/* access macros are not declared in non posix mode in unistd.h -
 don't try to use posix on NeXTstep 3.3 ! */
#include <libc.h>
#endif

/*@-redecl -redef@*/
/*@mayexit@*/ /*@only@*/ /*@unused@*/
char * xstrdup (const char *str)
	/*@*/;
/*@=redecl =redef@*/

#if defined(HAVE_MCHECK_H) && defined(__GNUC__)
#define	vmefail()	(fprintf(stderr, "virtual memory exhausted.\n"), exit(EXIT_FAILURE), NULL)
#define xstrdup(_str)   (strcpy((malloc(strlen(_str)+1) ? : vmefail()), (_str)))
#else
#define	xstrdup(_str)	strdup(_str)
#endif  /* defined(HAVE_MCHECK_H) && defined(__GNUC__) */

#if defined(HAVE___SECURE_GETENV) && !defined(__LCLINT__)
#define	getenv(_s)	__secure_getenv(_s)
#endif

#include "popt.h"
