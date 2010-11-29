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

#ifdef __GNUC__
# undef alloca
# define alloca __builtin_alloca
#else
# ifdef HAVE_ALLOCA_H
#  include <alloca.h>
# else
#  ifndef _AIX
/* AIX alloca decl has to be the first thing in the file, bletch! */
char *alloca ();
#  endif
# endif
#endif

#if defined(_MSC_VER) || defined(__STDC__)
#  define inline __inline
#endif /* _MSC_VER */

#ifdef _MSC_VER
/* missing unistd.h stuff */

#define F_OK 0
#define R_OK 4
#define W_OK 2
#define X_OK 1

#define srandom srand
#define random rand
#define access _access

/* Pretend to be root to replace these */
inline int setuid(int) { return 1; }
inline int getuid(void) { return 0; }

inline int seteuid(int) { return 1; }
inline int geteuid(void) { return 0; }

inline int setgid(int) { return 1; }
inline int getgid(void) { return 0; }

inline int setegid(int) { return 1; }

#endif


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

#if defined(HAVE_UNISTD_H) && !defined(__LCLINT__)
#include <unistd.h>
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#ifdef __NeXT
/* access macros are not declared in non posix mode in unistd.h -
 don't try to use posix on NeXTstep 3.3 ! */
#include <libc.h>
#endif

/*@-incondefs@*/
/*@mayexit@*/ /*@only@*/ /*@out@*/ /*@unused@*/
void * xmalloc (size_t size)
	/*@globals errno @*/
	/*@ensures maxSet(result) == (size - 1) @*/
	/*@modifies errno @*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
void * xcalloc (size_t nmemb, size_t size)
	/*@ensures maxSet(result) == (nmemb - 1) @*/
	/*@*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
void * xrealloc (/*@null@*/ /*@only@*/ void * ptr, size_t size)
	/*@ensures maxSet(result) == (size - 1) @*/
	/*@modifies *ptr @*/;

/*@mayexit@*/ /*@only@*/ /*@unused@*/
char * xstrdup (const char *str)
	/*@*/;
/*@=incondefs@*/

#if !defined(HAVE_STPCPY)
/* Copy SRC to DEST, returning the address of the terminating '\0' in DEST.  */
static inline char * stpcpy (char *dest, const char * src) {
    register char *d = dest;
    register const char *s = src;

    do
	*d++ = *s;
    while (*s++ != '\0');
    return d - 1;
}
#endif

/* Memory allocation via macro defs to get meaningful locations from mtrace() */
#if defined(HAVE_MCHECK_H) && defined(__GNUC__)
#define	vmefail()	(fprintf(stderr, "virtual memory exhausted.\n"), exit(EXIT_FAILURE), NULL)
#define	xmalloc(_size) 		(malloc(_size) ? : vmefail())
#define	xcalloc(_nmemb, _size)	(calloc((_nmemb), (_size)) ? : vmefail())
#define	xrealloc(_ptr, _size)	(realloc((_ptr), (_size)) ? : vmefail())
#define xstrdup(_str)   (strcpy((malloc(strlen(_str)+1) ? : vmefail()), (_str)))
#else
#define	xmalloc(_size) 		malloc(_size)
#define	xcalloc(_nmemb, _size)	calloc((_nmemb), (_size))
#define	xrealloc(_ptr, _size)	realloc((_ptr), (_size))
#define	xstrdup(_str)	strdup(_str)
#endif  /* defined(HAVE_MCHECK_H) && defined(__GNUC__) */

#if defined(HAVE___SECURE_GETENV) && !defined(__LCLINT__)
#define	getenv(_s)	__secure_getenv(_s)
#endif

#if !defined(__GNUC__) && !defined(__attribute__)
#define __attribute__(x)
#endif
#define UNUSED(x) x __attribute__((__unused__))

#include "popt.h"
