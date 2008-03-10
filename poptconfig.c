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
#endif

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
    char * file = NULL, * chptr, * end;
    char * buf = NULL;
/*@dependent@*/ char * dst;
    int fd, rc;
    off_t fileLength;

    fd = open(fn, O_RDONLY);
    if (fd < 0)
	return (errno == ENOENT ? 0 : POPT_ERROR_ERRNO);

    fileLength = lseek(fd, 0, SEEK_END);
    if (fileLength == (off_t)-1 || lseek(fd, 0, 0) == (off_t)-1) {
	rc = errno;
	(void) close(fd);
	errno = rc;
	return POPT_ERROR_ERRNO;
    }

    if ((file = malloc((size_t)fileLength + 1)) != NULL)
	*file = '\0';
    if (file == NULL
     || read(fd, (char *)file, (size_t)fileLength) != (ssize_t)fileLength)
    {
	rc = errno;
	(void) close(fd);
	errno = rc;
	if (file)
	    free(file);
	return POPT_ERROR_ERRNO;
    }
    if (close(fd) == -1) {
	free(file);
	return POPT_ERROR_ERRNO;
    }

    dst = buf = malloc((size_t)fileLength + 1);
    if (dst == NULL)
	return POPT_ERROR_ERRNO;

    end = (file + fileLength);
    for (chptr = file; chptr < end; chptr++) {
	switch (*chptr) {
	  case '\n':
	    *dst = '\0';
	    dst = buf;
	    while (*dst && _isspaceptr(dst)) dst++;
	    if (*dst && *dst != '#')
		configLine(con, dst);
	    /*@switchbreak@*/ break;
	  case '\\':
	    *dst = *chptr++;
	    /* \ at the end of a line does not insert a \n */
	    if (chptr < end  && *chptr != '\n') {
		dst++;
		*dst++ = *chptr;
	    }
	    /*@switchbreak@*/ break;
	  default:
	    *dst++ = *chptr;
	    /*@switchbreak@*/ break;
	}
    }

    free(file);
    free(buf);

    return 0;
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
	if (!glob("/etc/popt.d/*", 0, NULL, pglob)) {
            size_t i;
	    for (i = 0; i < pglob->gl_pathc; i++) {
		char * f = pglob->gl_pathv[i];
		if (strstr(f, ".rpmnew") || strstr(f, ".rpmsave"))
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
