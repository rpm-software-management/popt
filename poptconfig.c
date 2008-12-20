/** \ingroup popt
 * \file popt/poptconfig.c
 */

/* (C) 1998-2002 Red Hat, Inc. -- Licensing details are in the COPYING
   file accompanying popt source distributions, available from 
   ftp://ftp.rpm.org/pub/rpm/dist. */

#include "system.h"
#include "poptint.h"
#include <sys/stat.h>

#if defined(HAVE_GLOB_H)
#include <glob.h>

#if defined(__LCLINT__)
/*@-declundef -exportheader -incondefs -protoparammatch -redecl -type @*/
extern int glob (const char *__pattern, int __flags,
		/*@null@*/ int (*__errfunc) (const char *, int),
		/*@out@*/ glob_t *__pglob)
	/*@globals errno, fileSystem @*/
	/*@modifies *__pglob, errno, fileSystem @*/;

/* XXX only annotation is a white lie */
extern void globfree (/*@only@*/ glob_t *__pglob)
	/*@modifies *__pglob @*/;
/*@=declundef =exportheader =incondefs =protoparammatch =redecl =type @*/
#endif

/*@unchecked@*/
static int poptGlobFlags = 0;

static int poptGlob_pattern_p (const char *pattern, int quote)
	/*@*/
{
    const char * pat = pattern;
    int rc;
    /* XXX skip the attention marker. */
    if (pat[0] == '@' && pat[1] != '(')
	pat++;
    rc = glob_pattern_p(pat, quote);
    return rc;
}

static int poptGlob_error(/*@unused@*/ UNUSED(const char * epath),
		/*@unused@*/ UNUSED(int eerrno))
	/*@*/
{
    return 1;
}

static int poptGlob(const char * pattern, int flags,
		int errfunc(const char * epath, int eerrno),
		/*@out@*/ glob_t * pglob)
	/*@globals fileSystem @*/
	/*@modifies *pglob, fileSystem @*/
{
    const char * pat = pattern;
    int rc;
    /* XXX skip the attention marker. */
    if (pat[0] == '@' && pat[1] != '(')
	pat++;
    rc = glob(pat, flags, errfunc, pglob);
    return rc;
}

static void poptGlobfree( /*@only@*/ glob_t * pglob)
	/*@globals fileSystem @*/
	/*@modifies *pglob, fileSystem @*/
{
    globfree(pglob);
}
#endif	/* HAVE_GLOB_H */

/*@access poptContext @*/

/*@-compmempass@*/	/* FIX: item->option.longName kept, not dependent. */
static void configLine(poptContext con, char * line)
	/*@modifies con @*/
{
    size_t nameLength;
    const char * entryType;
    const char * opt;
    struct poptItem_s item_buf;
    poptItem item = &item_buf;
    int i, j;

    if (con->appName == NULL)
	return;
    nameLength = strlen(con->appName);
    
    memset(item, 0, sizeof(*item));

    if (strncmp(line, con->appName, nameLength)) return;

    line += nameLength;
    if (*line == '\0' || !_isspaceptr(line)) return;

    while (*line != '\0' && _isspaceptr(line)) line++;
    entryType = line;
    while (*line == '\0' || !_isspaceptr(line)) line++;
    *line++ = '\0';

    while (*line != '\0' && _isspaceptr(line)) line++;
    if (*line == '\0') return;
    opt = line;
    while (*line == '\0' || !_isspaceptr(line)) line++;
    *line++ = '\0';

    while (*line != '\0' && _isspaceptr(line)) line++;
    if (*line == '\0') return;

/*@-temptrans@*/ /* FIX: line alias is saved */
    if (opt[0] == '-' && opt[1] == '-')
	item->option.longName = opt + 2;
    else if (opt[0] == '-' && opt[2] == '\0')
	item->option.shortName = opt[1];
/*@=temptrans@*/

    if (poptParseArgvString(line, &item->argc, &item->argv)) return;

/*@-modobserver@*/
    item->option.argInfo = POPT_ARGFLAG_DOC_HIDDEN;
    for (i = 0, j = 0; i < item->argc; i++, j++) {
	const char * f;
	if (!strncmp(item->argv[i], "--POPTdesc=", sizeof("--POPTdesc=")-1)) {
	    f = item->argv[i] + sizeof("--POPTdesc=");
	    if (f[0] == '$' && f[1] == '"') f++;
	    item->option.descrip = f;
	    item->option.argInfo &= ~POPT_ARGFLAG_DOC_HIDDEN;
	    j--;
	} else
	if (!strncmp(item->argv[i], "--POPTargs=", sizeof("--POPTargs=")-1)) {
	    f = item->argv[i] + sizeof("--POPTargs=");
	    if (f[0] == '$' && f[1] == '"') f++;
	    item->option.argDescrip = f;
	    item->option.argInfo &= ~POPT_ARGFLAG_DOC_HIDDEN;
	    item->option.argInfo |= POPT_ARG_STRING;
	    j--;
	} else
	if (j != i)
	    item->argv[j] = item->argv[i];
    }
    if (j != i) {
	item->argv[j] = NULL;
	item->argc = j;
    }
/*@=modobserver@*/
	
/*@-nullstate@*/ /* FIX: item->argv[] may be NULL */
    if (!strcmp(entryType, "alias"))
	(void) poptAddItem(con, item, 0);
    else if (!strcmp(entryType, "exec"))
	(void) poptAddItem(con, item, 1);
/*@=nullstate@*/
}
/*@=compmempass@*/

int poptReadConfigFile(poptContext con, const char * fn)
{
    int fdno;
    char *b = NULL, *be;
    off_t nb;
    const char *se;
    char *t, *te;
    int rc = POPT_ERROR_ERRNO;	/* assume failure */

    fdno = open(fn, O_RDONLY);
    if (fdno < 0)
	return (errno == ENOENT ? 0 : rc);

    if ((nb = lseek(fdno, 0, SEEK_END)) == (off_t)-1
     || lseek(fdno, 0, SEEK_SET) == (off_t)-1
     || (b = malloc((size_t)nb + 1)) == NULL
     || read(fdno, (char *)b, (size_t)nb) != (ssize_t)nb)
    {
	int oerrno = errno;
	(void) close(fdno);
	errno = oerrno;
	goto exit;
    }
    if (close(fdno) == -1)
	goto exit;

    if ((t = malloc((size_t)nb + 1)) == NULL)
	goto exit;
    te = t;

    be = (b + nb);
    for (se = b; se < be; se++) {
	switch (*se) {
	  case '\n':
	    *te = '\0';
	    te = t;
	    while (*te && _isspaceptr(te)) te++;
	    if (*te && *te != '#')
		configLine(con, te);
	    /*@switchbreak@*/ break;
/*@-usedef@*/	/* XXX *se may be uninitialized */
	  case '\\':
	    *te = *se++;
	    /* \ at the end of a line does not insert a \n */
	    if (se < be  && *se != '\n') {
		te++;
		*te++ = *se;
	    }
	    /*@switchbreak@*/ break;
	  default:
	    *te++ = *se;
	    /*@switchbreak@*/ break;
/*@=usedef@*/
	}
    }

    free(t);
    rc = 0;

exit:
    if (b)
	free(b);
    return rc;
}

int poptSaneFile(const char * fn)
{
    struct stat sb;
    uid_t uid = getuid();

    if (stat(fn, &sb) == -1)
	return 1;
    if ((uid_t)sb.st_uid != uid)
	return 0;
    if (!S_ISREG(sb.st_mode))
	return 0;
/*@-bitwisesigned@*/
    if (sb.st_mode & (S_IWGRP|S_IWOTH))
	return 0;
/*@=bitwisesigned@*/
    return 1;
}

int poptReadConfigFiles(poptContext con, const char * paths)
{
    char * buf = (paths ? xstrdup(paths) : NULL);
    const char * p;
    char * pe;
    int rc = 0;		/* assume success */

    for (p = buf; p != NULL && *p != '\0'; p = pe) {
	const char ** av = NULL;
	size_t ac = 0;
	size_t i;
	int xx;

	/* locate start of next path element */
	pe = strchr(p, ':');
	if (pe != NULL && *pe == ':')
	    *pe++ = '\0';
	else
	    pe = (char *) (p + strlen(p));

#if defined(HAVE_GLOB_H)
	if (poptGlob_pattern_p(p, 0)) {
	    glob_t _g, *pglob = &_g;

	    if ((xx = poptGlob(p, poptGlobFlags, poptGlob_error, pglob)) != 0)
		continue;
	    for (i = 0; i < pglob->gl_pathc; i++) {
		char * fn = pglob->gl_pathv[i];
		/* XXX should '@' attention be pushed into poptReadConfigFile? */
		if (fn[0] == '@' && fn[1] != '(')
		    fn++;
		xx = poptReadConfigFile(con, fn);
		if (xx && rc == 0)
		    rc = xx;
	    }
	    poptGlobfree(pglob);
	} else
#endif
      {
	ac = (size_t)1;
	if ((av = calloc(ac + 1, sizeof(*av))) == NULL)
	    continue;	/* XXX error return */
	av[0] = xstrdup(p);
	/* work-off each resulting file from the path element */
	for (i = 0; i < ac; i++) {
	    const char * fn = av[i];
	    if (av[i] == NULL)	/* XXX can't happen */
		/*@innercontinue@*/ continue;
	    /* XXX should '@' attention be pushed into poptReadConfigFile? */
	    if (fn[0] == '@' && fn[1] != '(') {
		fn++;
		xx = poptSaneFile(fn);
		if (!xx && rc == 0)
		    rc = POPT_ERROR_BADCONFIG;
		/*@innercontinue@*/ continue;
	    }
	    xx = poptReadConfigFile(con, fn);
	    if (xx && rc == 0)
		rc = xx;
	    free((void *)av[i]);
	    av[i] = NULL;
	}
	free(av);
	av = NULL;
      }
    }

/*@-usedef@*/
    if (buf)
	free(buf);
/*@=usedef@*/

    return rc;
}

int poptReadDefaultConfig(poptContext con, /*@unused@*/ UNUSED(int useEnv))
{
    static const char _popt_sysconfdir[] = POPT_SYSCONFDIR "/popt";
    static const char _popt_etc[] = "/etc/popt";
    char * fn, * home;
    struct stat s;
    int rc;

    if (con->appName == NULL) return 0;

    if (strcmp(_popt_sysconfdir, _popt_etc)) {
	rc = poptReadConfigFile(con, _popt_sysconfdir);
	if (rc) return rc;
    }

    rc = poptReadConfigFile(con, _popt_etc);
    if (rc) return rc;

#if defined(HAVE_GLOB_H)
    if (!stat("/etc/popt.d", &s) && S_ISDIR(s.st_mode)) {
	glob_t _g, *pglob = &_g;
	if (!glob("/etc/popt.d/*", poptGlobFlags, poptGlob_error, pglob)) {
	    size_t i;
	    for (i = 0; i < pglob->gl_pathc; i++) {
		char * f = pglob->gl_pathv[i];
		if (f == NULL || strstr(f, ".rpmnew") || strstr(f, ".rpmsave"))
		    continue;
		if (!stat(f, &s)) {
		    if (!S_ISREG(s.st_mode) && !S_ISLNK(s.st_mode))
			continue;
		}
		rc = poptReadConfigFile(con, f);
		if (rc) return rc;
	    }
	    globfree(pglob);
	}
    }
#endif

    if ((home = getenv("HOME"))) {
	fn = malloc(strlen(home) + 20);
	if (fn != NULL) {
	    (void) stpcpy(stpcpy(fn, home), "/.popt");
	    rc = poptReadConfigFile(con, fn);
	    free(fn);
	} else
	    rc = POPT_ERROR_ERRNO;
	if (rc) return rc;
    }

    return 0;
}

poptContext
poptFini(poptContext con)
{
    return poptFreeContext(con);
}

poptContext
poptInit(int argc, const char ** argv,
		const struct poptOption * options, const char * configPaths)
{
    poptContext con = NULL;
    const char * argv0;

    if (argv == NULL || argv[0] == NULL || options == NULL)
	return con;

    if ((argv0 = strrchr(argv[0], '/')) != NULL) argv0++;
    else argv0 = argv[0];
   
    con = poptGetContext(argv0, argc, (const char **)argv, options, 0);
    if (con != NULL&& poptReadConfigFiles(con, configPaths))
	con = poptFini(con);

    return con;
}
