/** \ingroup popt
 * \file popt/popt.c
 */

/* (C) 1998-2002 Red Hat, Inc. -- Licensing details are in the COPYING
   file accompanying popt source distributions, available from
   ftp://ftp.rpm.org/pub/rpm/dist */

#undef	MYDEBUG

#include "system.h"

#if defined(__LCLINT__)
#ifndef _MSC_VER
/*@-declundef -exportheader @*/
extern long long int strtoll(const char *nptr, /*@null@*/ char **endptr,
		int base)
	/*@modifies *endptr@*/;
/*@=declundef =exportheader @*/

#else
  #define strtoll _strtoi64
#endif /* _MSC_VER */
#endif /* defined(__LCLINT__) */

#ifdef HAVE_FLOAT_H
#include <float.h>
#endif
#include <math.h>

#include "poptint.h"
#if defined(HAVE_ASSERT_H)
#include <assert.h>
#else
#define	assert(_x)
#endif

#ifdef	MYDEBUG
/*@unchecked@*/
int _popt_debug = 0;
#endif

/*@unchecked@*/
unsigned int _poptArgMask = POPT_ARG_MASK;
/*@unchecked@*/
unsigned int _poptGroupMask = POPT_GROUP_MASK;

#if !defined(HAVE_STRERROR) && !defined(__LCLINT__)
static char * strerror(int errno)
{
    extern int sys_nerr;
    extern char * sys_errlist[];

    if ((0 <= errno) && (errno < sys_nerr))
	return sys_errlist[errno];
    else
	return POPT_("unknown errno");
}
#endif

#ifdef MYDEBUG
/*@unused@*/
static void prtcon(const char *msg, poptContext con)
{
    if (msg) fprintf(stderr, "%s", msg);
    fprintf(stderr, "\tcon %p os %p nextCharArg \"%s\" nextArg \"%s\" argv[%d] \"%s\"\n",
	con, con->os,
	(con->os->nextCharArg ? con->os->nextCharArg : ""),
	(con->os->nextArg ? con->os->nextArg : ""),
	con->os->next,
	(con->os->argv && con->os->argv[con->os->next]
		? con->os->argv[con->os->next] : ""));
}
#endif

void poptSetExecPath(poptContext con, const char * path, int allowAbsolute)
{
    con->execPath = _free(con->execPath);
    con->execPath = xstrdup(path);
    con->execAbsolute = allowAbsolute;
    return;
}

static void invokeCallbacksPRE(poptContext con, const struct poptOption * opt)
	/*@globals internalState@*/
	/*@modifies internalState@*/
{
    if (opt != NULL)
    for (; opt->longName || opt->shortName || opt->arg; opt++) {
    poptArg arg;
    arg.ptr = opt->arg;
	if (arg.ptr)
	switch (poptArgType(opt)) {
	case POPT_ARG_INCLUDE_TABLE:	/* Recurse on included sub-tables. */
	    poptSubstituteHelpI18N(arg.opt);	/* XXX side effects */
	    invokeCallbacksPRE(con, arg.opt);
	    /*@switchbreak@*/ break;
	case POPT_ARG_CALLBACK:		/* Perform callback. */
	    if (!CBF_ISSET(opt, PRE))
		/*@switchbreak@*/ break;
/*@-noeffectuncon @*/	/* XXX no known way to annotate (*vector) calls. */
	    arg.cb(con, POPT_CALLBACK_REASON_PRE, NULL, NULL, opt->descrip);
/*@=noeffectuncon @*/
	    /*@switchbreak@*/ break;
	}
    }
}

static void invokeCallbacksPOST(poptContext con, const struct poptOption * opt)
	/*@globals internalState@*/
	/*@modifies internalState@*/
{
    if (opt != NULL)
    for (; opt->longName || opt->shortName || opt->arg; opt++) {
    poptArg arg;
    arg.ptr = opt->arg;
	if (arg.ptr)
	switch (poptArgType(opt)) {
	case POPT_ARG_INCLUDE_TABLE:	/* Recurse on included sub-tables. */
	    poptSubstituteHelpI18N(arg.opt);	/* XXX side effects */
	    invokeCallbacksPOST(con, arg.opt);
	    /*@switchbreak@*/ break;
	case POPT_ARG_CALLBACK:		/* Perform callback. */
	    if (!CBF_ISSET(opt, POST))
		/*@switchbreak@*/ break;
/*@-noeffectuncon @*/	/* XXX no known way to annotate (*vector) calls. */
	    arg.cb(con, POPT_CALLBACK_REASON_POST, NULL, NULL, opt->descrip);
/*@=noeffectuncon @*/
	    /*@switchbreak@*/ break;
	}
    }
}

static void invokeCallbacksOPTION(poptContext con,
				const struct poptOption * opt,
				const struct poptOption * myOpt,
				/*@null@*/ const void * myData, int shorty)
	/*@globals internalState@*/
	/*@modifies internalState@*/
{
    const struct poptOption * cbopt = NULL;
    poptArg cbarg;
    cbarg.ptr = NULL;

    if (opt != NULL)
    for (; opt->longName || opt->shortName || opt->arg; opt++) {
    poptArg arg;
    arg.ptr = opt->arg;
	switch (poptArgType(opt)) {
	case POPT_ARG_INCLUDE_TABLE:	/* Recurse on included sub-tables. */
	    poptSubstituteHelpI18N(arg.opt);	/* XXX side effects */
	    if (opt->arg != NULL)
		invokeCallbacksOPTION(con, opt->arg, myOpt, myData, shorty);
	    /*@switchbreak@*/ break;
	case POPT_ARG_CALLBACK:		/* Save callback info. */
	    if (CBF_ISSET(opt, SKIPOPTION))
		/*@switchbreak@*/ break;
	    cbopt = opt;
	    cbarg.ptr = opt->arg;
	    /*@switchbreak@*/ break;
	default:		/* Perform callback on matching option. */
	    if (cbopt == NULL || cbarg.cb == NULL)
		/*@switchbreak@*/ break;
	    if ((myOpt->shortName && opt->shortName && shorty &&
			myOpt->shortName == opt->shortName)
	     || (myOpt->longName != NULL && opt->longName != NULL &&
			!strcmp(myOpt->longName, opt->longName)))
	    {	const void *cbData = (cbopt->descrip ? cbopt->descrip : myData);
/*@-noeffectuncon @*/	/* XXX no known way to annotate (*vector) calls. */
		cbarg.cb(con, POPT_CALLBACK_REASON_OPTION,
			myOpt, con->os->nextArg, cbData);
/*@=noeffectuncon @*/
		/* Terminate (unless explcitly continuing). */
		if (!CBF_ISSET(cbopt, CONTINUE))
		    return;
	    }
	    /*@switchbreak@*/ break;
	}
    }
}

poptContext poptGetContext(const char * name, int argc, const char ** argv,
			const struct poptOption * options, unsigned int flags)
{
    poptContext con = (poptContext) xcalloc(1, sizeof(*con));

assert(con);	/* XXX can't happen */
    if (con == NULL) return NULL;

    con->optionDepth = POPTINT_OPTION_DEPTH;
    con->os = con->optionStack;

#ifdef	SUPPORT_GLOBAL_CALCULATOR
    con->calcDepth = POPTINT_CALC_DEPTH;
    con->stk = con->calcStack;
#endif

    con->os->argc = argc;
/*@-dependenttrans -assignexpose@*/	/* FIX: W2DO? */
    con->os->argv = argv;
/*@=dependenttrans =assignexpose@*/
    con->os->argb = NULL;

    if (!(flags & POPT_CONTEXT_KEEP_FIRST))
	con->os->next = 1;		/* skip argv[0] */

    con->leftovers = (poptArgv) xcalloc( (size_t)(argc + 1), sizeof(*con->leftovers) );

/*@-dependenttrans -assignexpose@*/	/* FIX: W2DO? */
    con->options = options;
/*@=dependenttrans =assignexpose@*/

    con->aliases = NULL;
    con->numAliases = 0;

    con->flags = flags;
    con->execs = NULL;
    con->numExecs = 0;

    con->nav = argc * 2;
    con->av = (poptArgv) xcalloc( (size_t)con->nav, sizeof(*con->av) );
    con->execAbsolute = 1;
    con->arg_strip = NULL;

    if (getenv("POSIXLY_CORRECT") || getenv("POSIX_ME_HARDER"))
	con->flags |= POPT_CONTEXT_POSIXMEHARDER;

    if (name)
	con->appName = xstrdup(name);

    invokeCallbacksPRE(con, con->options);

    return con;
}

static void cleanOSE(/*@special@*/ struct optionStackEntry *os)
	/*@uses os @*/
	/*@releases os->nextArg, os->argv, os->argb @*/
	/*@modifies os @*/
{
    os->nextArg = _free(os->nextArg);
    os->argv = _free(os->argv);
    os->argb = PBM_FREE(os->argb);
}

void poptResetContext(poptContext con)
{
    if (con == NULL) return;
    while (con->os > con->optionStack) {
	cleanOSE(con->os--);
    }
    con->os->argb = PBM_FREE(con->os->argb);
    con->os->currAlias = NULL;
    con->os->nextCharArg = NULL;
    con->os->nextArg = NULL;
    con->os->next = 1;			/* skip argv[0] */

    con->numLeftovers = 0;
    con->nextLeftover = 0;
    con->restLeftover = 0;
    con->doExec = NULL;

    if (con->av != NULL) {
    int i;
     for (i = 0; i < con->ac; i++) {
/*@-unqualifiedtrans@*/		/* FIX: typedef double indirection. */
 	con->av[i] = _free(con->av[i]);
/*@=unqualifiedtrans@*/
     }
    }

    con->ac = 0;
    con->arg_strip = PBM_FREE(con->arg_strip);
/*@-nullstate@*/	/* FIX: con->av != NULL */
    return;
/*@=nullstate@*/
}

/* Only one of longName, shortName should be set, not both. */
static int handleExec(/*@special@*/ poptContext con,
		/*@null@*/ const char * longName, char shortName)
	/*@uses con->execs, con->numExecs, con->flags, con->doExec,
		con->av, con->nav, con->ac @*/
	/*@modifies con @*/
{
    poptItem item;
    int i;

    if (con->execs == NULL || con->numExecs <= 0)
	return 0;

    for (i = con->numExecs - 1; i >= 0; i--) {
	item = con->execs + i;
	if (longName && !(item->option.longName &&
			!strcmp(longName, item->option.longName)))
	    continue;
	else if (shortName != item->option.shortName)
	    continue;
	break;
    }
    if (i < 0) return 0;


    if (con->flags & POPT_CONTEXT_NO_EXEC)
	return 1;

    if (con->doExec == NULL) {
	con->doExec = con->execs + i;
	return 1;
    }

    /* We already have an exec to do; remember this option for next
       time 'round */
    if ((con->ac + 1) >= (con->nav)) {
	con->nav += 10;
	con->av = (poptArgv) xrealloc(con->av, sizeof(*con->av) * con->nav);
    }

    i = con->ac++;
assert(con->av);		/* XXX can't happen */
    if (con->av != NULL)
    {	char *s  = (char*) xmalloc((longName ? strlen(longName) : 0) + sizeof("--"));
assert(s);	/* XXX can't happen */
	if (s != NULL) {
	    con->av[i] = s;
	    *s++ = '-';
	    if (longName)
		s = stpcpy( stpcpy(s, "-"), longName);
	    else
		*s++ = shortName;
	    *s = '\0';
	} else
	    con->av[i] = NULL;
    }

    return 1;
}

/**
 * Compare long option for equality, adjusting for POPT_ARGFLAG_TOGGLE.
 * @param opt           option
 * @param longName	arg option
 * @param longNameLen	arg option length
 * @return		does long option match?
 */
static int
longOptionStrcmp(const struct poptOption * opt,
		/*@null@*/ const char * longName, size_t longNameLen)
	/*@*/
{
    const char * optLongName = opt->longName;
    int rc;

assert(optLongName && longName);	/* XXX can't heppen */
    if (optLongName == NULL || longName == NULL)
	return 0;

    if (F_ISSET(opt, TOGGLE)) {
	if (optLongName[0] == 'n' && optLongName[1] == 'o') {
	    optLongName += sizeof("no") - 1;
	    if (optLongName[0] == '-')
		optLongName++;
	}
	if (longName[0] == 'n' && longName[1] == 'o') {
	    longName += sizeof("no") - 1;
	    longNameLen -= sizeof("no") - 1;
	    if (longName[0] == '-') {
		longName++;
		longNameLen--;
	    }
	}
    }
    rc = (int)(strlen(optLongName) == longNameLen);
    if (rc)
	rc = (int)(strncmp(optLongName, longName, longNameLen) == 0);
    return rc;
}

/* Only one of longName, shortName may be set at a time */
static int handleAlias(/*@special@*/ poptContext con,
		/*@null@*/ const char * longName, size_t longNameLen,
		char shortName,
		/*@exposed@*/ /*@null@*/ const char * nextArg)
	/*@uses con->aliases, con->numAliases, con->optionStack, con->os,
		con->os->currAlias, con->os->currAlias->option.longName @*/
	/*@modifies con @*/
{
    poptItem item = con->os->currAlias;
    int rc;
    int i;

    if (item) {
	if (longName && item->option.longName != NULL
	 && longOptionStrcmp(&item->option, longName, longNameLen))
	    return 0;
	else
	if (shortName && shortName == item->option.shortName)
	    return 0;
    }

    if (con->aliases == NULL || con->numAliases <= 0)
	return 0;

    for (i = con->numAliases - 1; i >= 0; i--) {
	item = con->aliases + i;
	if (longName) {
	    if (item->option.longName == NULL)
		continue;
	    if (!longOptionStrcmp(&item->option, longName, longNameLen))
		continue;
	} else if (shortName != item->option.shortName)
	    continue;
	break;
    }
    if (i < 0) return 0;

    if ((con->os - con->optionStack + 1) == con->optionDepth)
	return POPT_ERROR_OPTSTOODEEP;

    if (longName == NULL && nextArg != NULL && *nextArg != '\0')
	con->os->nextCharArg = nextArg;

    con->os++;
    con->os->next = 0;
    con->os->stuffed = 0;
    con->os->nextArg = NULL;
    con->os->nextCharArg = NULL;
    con->os->currAlias = con->aliases + i;
    {	const char ** av;
	int ac = con->os->currAlias->argc;
	/* Append --foo=bar arg to alias argv array (if present). */
	if (longName && nextArg != NULL && *nextArg != '\0') {
	    av = (const char**) xmalloc((ac + 1 + 1) * sizeof(*av));
assert(av);	/* XXX won't happen. */
	    if (av != NULL) {
		for (i = 0; i < ac; i++) {
		    av[i] = con->os->currAlias->argv[i];
		}
		av[ac++] = nextArg;
		av[ac] = NULL;
	    } else	/* XXX revert to old popt behavior if malloc fails. */
		av = con->os->currAlias->argv;
	} else
	    av = con->os->currAlias->argv;
	rc = poptDupArgv(ac, av, &con->os->argc, &con->os->argv);
	if (av != NULL && av != con->os->currAlias->argv)
	    free(av);
    }
    con->os->argb = NULL;

    return (rc ? rc : 1);
}

/**
 * Return absolute path to executable by searching PATH.
 * @param argv0		name of executable
 * @return		(malloc'd) absolute path to executable (or NULL)
 */
static /*@null@*/
const char * findProgramPath(/*@null@*/ const char * argv0)
	/*@*/
{
    char *path = NULL, *s = NULL, *se;
    char *t = NULL;

assert(argv0);	/* XXX can't happen */
    if (argv0 == NULL) return NULL;

    /* If there is a / in argv[0], it has to be an absolute path. */
    /* XXX Hmmm, why not if (argv0[0] == '/') ... instead? */
    if (strchr(argv0, '/'))
	return xstrdup(argv0);

    if ((path = getenv("PATH")) == NULL || (path = xstrdup(path)) == NULL)
	return NULL;

    /* The return buffer in t is big enough for any path. */
    if ((t = (char*) xmalloc(strlen(path) + strlen(argv0) + sizeof("/"))) != NULL)
    for (s = path; s && *s; s = se) {

	/* Snip PATH element into [s,se). */
	if ((se = strchr(s, ':')))
	    *se++ = '\0';

	/* Append argv0 to PATH element. */
	(void) stpcpy(stpcpy(stpcpy(t, s), "/"), argv0);

	/* If file is executable, bingo! */
	if (!access(t, X_OK))
	    break;
    }

    /* If no executable was found in PATH, return NULL. */
/*@-compdef@*/
    if (!(s && *s) && t != NULL)
	t = _free(t);
/*@=compdef@*/
/*@-modobserver -observertrans -usedef @*/
    path = _free(path);
/*@=modobserver =observertrans =usedef @*/

    return t;
}

static int execCommand(poptContext con)
	/*@globals internalState @*/
	/*@modifies internalState @*/
{
    poptItem item = con->doExec;
    poptArgv argv = NULL;
    int argc = 0;
    int rc;
    int ec = POPT_ERROR_ERRNO;

assert(item);	/*XXX can't happen*/
    if (item == NULL)
	return POPT_ERROR_NOARG;

    if (item->argv == NULL || item->argc < 1 ||
	(!con->execAbsolute && strchr(item->argv[0], '/')))
	    return POPT_ERROR_NOARG;

    argv = (poptArgv) xmalloc(sizeof(*argv) *
			(6 + item->argc + con->numLeftovers + con->ac));
assert(argv);	/* XXX can't happen */
    if (argv == NULL) return POPT_ERROR_MALLOC;

    if (!strchr(item->argv[0], '/') && con->execPath != NULL) {
        char *s = (char*) xmalloc(strlen(con->execPath) + strlen(item->argv[0]) + sizeof("/"));
	if (s)
	    (void)stpcpy(stpcpy(stpcpy(s, con->execPath), "/"), item->argv[0]);

	argv[argc] = s;
    } else
	argv[argc] = findProgramPath(item->argv[0]);
    if (argv[argc++] == NULL) {
	ec = POPT_ERROR_NOARG;
	goto exit;
    }

    if (item->argc > 1) {
	memcpy(argv + argc, item->argv + 1, sizeof(*argv) * (item->argc - 1));
	argc += (item->argc - 1);
    }

    if (con->av != NULL && con->ac > 0) {
	memcpy(argv + argc, con->av, sizeof(*argv) * con->ac);
	argc += con->ac;
    }

    if (con->leftovers != NULL && con->numLeftovers > 0) {
	memcpy(argv + argc, con->leftovers, sizeof(*argv) * con->numLeftovers);
	argc += con->numLeftovers;
    }

    argv[argc] = NULL;

#if defined(hpux) || defined(__hpux)
    rc = setresgid(getgid(), getgid(),-1);
    if (rc) goto exit;
    rc = setresuid(getuid(), getuid(),-1);
    if (rc) goto exit;
#else
/*
 * XXX " ... on BSD systems setuid() should be preferred over setreuid()"
 * XXX 	sez' Timur Bakeyev <mc@bat.ru>
 * XXX	from Norbert Warmuth <nwarmuth@privat.circular.de>
 */
#if defined(HAVE_SETUID)
    rc = setgid(getgid());
    if (rc) goto exit;
    rc = setuid(getuid());
    if (rc) goto exit;
#elif defined (HAVE_SETREUID)
    rc = setregid(getgid(), getgid());
    if (rc) goto exit;
    rc = setreuid(getuid(), getuid());
    if (rc) goto exit;
#else
    ; /* Can't drop privileges */
#endif
#endif

#ifdef	MYDEBUG
if (_popt_debug)
    {	poptArgv avp;
	fprintf(stderr, "==> execvp(%s) argv[%d]:", argv[0], argc);
	for (avp = argv; *avp; avp++)
	    fprintf(stderr, " '%s'", *avp);
	fprintf(stderr, "\n");
    }
#endif

/*@-nullstate@*/
    rc = execvp(argv[0], (char *const *)argv);
/*@=nullstate@*/

exit:
    if (argv) {
        if (argv[0])
            free((void *)argv[0]);
        free(argv);
    }
    return ec;
}

/*@observer@*/ /*@null@*/
static const struct poptOption *
findOption(const struct poptOption * opt,
		/*@null@*/ const char * longName, size_t longNameLen,
		char shortName,
		/*@null@*/ /*@out@*/ poptCallbackType * callback,
		/*@null@*/ /*@out@*/ const void ** callbackData,
		unsigned int argInfo)
	/*@modifies *callback, *callbackData */
{
    const struct poptOption * cb = NULL;
    poptArg cbarg;
    cbarg.ptr = NULL;

    /* This happens when a single - is given */
    if (LF_ISSET(ONEDASH) && !shortName && (longName && *longName == '\0'))
	shortName = '-';

    for (; opt->longName || opt->shortName || opt->arg; opt++) {
        poptArg arg;
        arg.ptr = opt->arg;

	switch (poptArgType(opt)) {
	case POPT_ARG_INCLUDE_TABLE:	/* Recurse on included sub-tables. */
	{   const struct poptOption * opt2;

	    poptSubstituteHelpI18N(arg.opt);	/* XXX side effects */
	    if (arg.ptr == NULL) continue;	/* XXX program error */
	    opt2 = findOption(arg.opt, longName, longNameLen, shortName, callback,
			      callbackData, argInfo);
	    if (opt2 == NULL) continue;
	    /* Sub-table data will be inheirited if no data yet. */
/*@-observertrans -dependenttrans @*/
	    if (callback && *callback
	     && callbackData && *callbackData == NULL)
		*callbackData = opt->descrip;
/*@=observertrans =dependenttrans @*/
	    return opt2;
	}   /*@notreached@*/ /*@switchbreak@*/ break;
	case POPT_ARG_CALLBACK:
	    cb = opt;
	    cbarg.ptr = opt->arg;
	    continue;
	    /*@notreached@*/ /*@switchbreak@*/ break;
	default:
	    /*@switchbreak@*/ break;
	}

	if (longName != NULL && opt->longName != NULL &&
		   (!LF_ISSET(ONEDASH) || F_ISSET(opt, ONEDASH)) &&
		   longOptionStrcmp(opt, longName, longNameLen))
	{
	    break;
	} else if (shortName && shortName == opt->shortName) {
	    break;
	}
    }

    if (opt->longName == NULL && !opt->shortName)
	return NULL;

/*@-modobserver -mods @*/
    if (callback)
	*callback = (cb ? cbarg.cb : NULL);
    if (callbackData)
/*@-observertrans -dependenttrans @*/
	*callbackData = (cb && !CBF_ISSET(cb, INC_DATA) ? cb->descrip : NULL);
/*@=observertrans =dependenttrans @*/
/*@=modobserver =mods @*/

    return opt;
}

static const char * findNextArg(/*@special@*/ poptContext con,
		unsigned argx, int delete_arg)
	/*@uses con->optionStack, con->os,
		con->os->next, con->os->argb, con->os->argc, con->os->argv @*/
	/*@modifies con @*/
{
    struct optionStackEntry * os = con->os;
    const char * arg;

    do {
	int i;
	arg = NULL;
	while (os->next == os->argc && os > con->optionStack) os--;
	if (os->next == os->argc && os == con->optionStack) break;
	if (os->argv != NULL)
	for (i = os->next; i < os->argc; i++) {
/*@-sizeoftype@*/
	    if (os->argb && PBM_ISSET(i, os->argb))
		/*@innercontinue@*/ continue;
	    if (*os->argv[i] == '-')
		/*@innercontinue@*/ continue;
	    if (--argx > 0)
		/*@innercontinue@*/ continue;
	    arg = os->argv[i];
	    if (delete_arg) {
		if (os->argb == NULL) os->argb = PBM_ALLOC(os->argc);
assert(os->argb);	/* XXX can't happen */
		if (os->argb != NULL)
		    PBM_SET(i, os->argb);
	    }
	    /*@innerbreak@*/ break;
/*@=sizeoftype@*/
	}
	if (os > con->optionStack) os--;
    } while (arg == NULL);
    return arg;
}

static /*@only@*/ /*@null@*/ const char *
expandNextArg(/*@special@*/ poptContext con, const char * s)
	/*@uses con->optionStack, con->os,
		con->os->next, con->os->argb, con->os->argc, con->os->argv @*/
	/*@modifies con @*/
{
    const char * a = NULL;
    char *t, *te;
    size_t tn = strlen(s) + 1;
    char c;

    te = t = (char*) xmalloc(tn);
assert(t);	/* XXX can't happen */
    if (t == NULL) return NULL;
    *t = '\0';
    while ((c = *s++) != '\0') {
	switch (c) {
#if 0	/* XXX can't do this */
	case '\\':	/* escape */
	    c = *s++;
	    /*@switchbreak@*/ break;
#endif
	case '!':
	    if (!(s[0] == '#' && s[1] == ':' && s[2] == '+'))
		/*@switchbreak@*/ break;
	    /* XXX Make sure that findNextArg deletes only next arg. */
	    if (a == NULL) {
		if ((a = findNextArg(con, 1U, 1)) == NULL)
		    /*@switchbreak@*/ break;
	    }
	    s += sizeof("#:+") - 1;

	    tn += strlen(a);
	    {   size_t pos = (size_t) (te - t);
        t = (char*) xrealloc(t, tn);
assert(t);	/* XXX can't happen */
		if (t == NULL)
		    return NULL;
		te = stpcpy(t + pos, a);
	    }
	    continue;
	    /*@notreached@*/ /*@switchbreak@*/ break;
	default:
	    /*@switchbreak@*/ break;
	}
	*te++ = c;
    }
    *te++ = '\0';
    /* If the new string is longer than needed, shorten. */
    if ((t + tn) > te) {
/*@-usereleased@*/	/* XXX splint can't follow the pointers. */
    if ((te = (char*) xrealloc(t, (size_t)(te - t))) == NULL)
	    free(t);
	t = te;
/*@=usereleased@*/
    }
    return t;
}

static void poptStripArg(/*@special@*/ poptContext con, int which)
	/*@uses con->optionStack @*/
	/*@defines con->arg_strip @*/
	/*@modifies con @*/
{
/*@-compdef -sizeoftype -usedef @*/
    if (con->arg_strip == NULL)
	con->arg_strip = PBM_ALLOC(con->optionStack[0].argc);
assert(con->arg_strip);		/* XXX can't happen */
    if (con->arg_strip != NULL)
    PBM_SET(which, con->arg_strip);
    return;
/*@=compdef =sizeoftype =usedef @*/
}

/*@unchecked@*/
unsigned int _poptBitsN = _POPT_BITS_N;
/*@unchecked@*/
unsigned int _poptBitsM = _POPT_BITS_M;
/*@unchecked@*/
unsigned int _poptBitsK = _POPT_BITS_K;

/*@-sizeoftype@*/
static int _poptBitsNew(/*@null@*/ poptBits *bitsp)
	/*@globals _poptBitsN, _poptBitsM, _poptBitsK @*/
	/*@modifies *bitsp, _poptBitsN, _poptBitsM, _poptBitsK @*/
{
    if (bitsp == NULL)
	return POPT_ERROR_NULLARG;

    /* XXX handle negated initialization. */
    if (*bitsp == NULL) {
	if (_poptBitsN == 0) {
	    _poptBitsN = _POPT_BITS_N;
	    _poptBitsM = _POPT_BITS_M;
	}
	if (_poptBitsM == 0U) _poptBitsM = (3 * _poptBitsN) / 2;
	if (_poptBitsK == 0U || _poptBitsK > 32U) _poptBitsK = _POPT_BITS_K;
	*bitsp = PBM_ALLOC(_poptBitsM-1);
    }
/*@-nullstate@*/
    return 0;
/*@=nullstate@*/
}

int poptBitsAdd(poptBits bits, const char * s)
{
    size_t ns = (s ? strlen(s) : 0);
    uint32_t h0 = 0;
    uint32_t h1 = 0;

    if (bits == NULL || ns == 0)
	return POPT_ERROR_NULLARG;

    poptJlu32lpair(s, ns, &h0, &h1);

    for (ns = 0; ns < (size_t)_poptBitsK; ns++) {
        uint32_t h = h0 + ns * h1;
        uint32_t ix = (h % _poptBitsM);
        PBM_SET(ix, bits);
    }
    return 0;
}

int poptBitsChk(poptBits bits, const char * s)
{
    size_t ns = (s ? strlen(s) : 0);
    uint32_t h0 = 0;
    uint32_t h1 = 0;
    int rc = 1;

    if (bits == NULL || ns == 0)
	return POPT_ERROR_NULLARG;

    poptJlu32lpair(s, ns, &h0, &h1);

    for (ns = 0; ns < (size_t)_poptBitsK; ns++) {
        uint32_t h = h0 + ns * h1;
        uint32_t ix = (h % _poptBitsM);
        if (PBM_ISSET(ix, bits))
            continue;
        rc = 0;
        break;
    }
    return rc;
}

int poptBitsClr(poptBits bits)
{
    static size_t nbw = (__PBM_NBITS/8);
    size_t nw = (__PBM_IX(_poptBitsM-1) + 1);

    if (bits == NULL)
	return POPT_ERROR_NULLARG;
    memset(bits, 0, nw * nbw);
    return 0;
}

int poptBitsDel(poptBits bits, const char * s)
{
    size_t ns = (s ? strlen(s) : 0);
    uint32_t h0 = 0;
    uint32_t h1 = 0;

    if (bits == NULL || ns == 0)
	return POPT_ERROR_NULLARG;

    poptJlu32lpair(s, ns, &h0, &h1);

    for (ns = 0; ns < (size_t)_poptBitsK; ns++) {
        uint32_t h = h0 + ns * h1;
        uint32_t ix = (h % _poptBitsM);
        PBM_CLR(ix, bits);
    }
    return 0;
}

int poptBitsIntersect(poptBits *ap, const poptBits b)
{
    __pbm_bits *abits;
    __pbm_bits *bbits;
    __pbm_bits rc = 0;
    size_t nw = (__PBM_IX(_poptBitsM-1) + 1);
    size_t i;

    if (ap == NULL || b == NULL || _poptBitsNew(ap))
	return POPT_ERROR_NULLARG;
    abits = __PBM_BITS(*ap);
    bbits = __PBM_BITS(b);

    for (i = 0; i < nw; i++) {
        abits[i] &= bbits[i];
	rc |= abits[i];
    }
    return (rc ? 1 : 0);
}

int poptBitsUnion(poptBits *ap, const poptBits b)
{
    __pbm_bits *abits;
    __pbm_bits *bbits;
    __pbm_bits rc = 0;
    size_t nw = (__PBM_IX(_poptBitsM-1) + 1);
    size_t i;

    if (ap == NULL || b == NULL || _poptBitsNew(ap))
	return POPT_ERROR_NULLARG;
    abits = __PBM_BITS(*ap);
    bbits = __PBM_BITS(b);

    for (i = 0; i < nw; i++) {
        abits[i] |= bbits[i];
	rc |= abits[i];
    }
    return (rc ? 1 : 0);
}

int poptBitsArgs(poptContext con, poptBits *ap)
{
    const char ** av;
    int rc = 0;

    if (con == NULL || ap == NULL || _poptBitsNew(ap) ||
	con->leftovers == NULL || con->numLeftovers == con->nextLeftover)
	return POPT_ERROR_NULLARG;

    /* some apps like [like RPM ;-) ] need this NULL terminated */
    con->leftovers[con->numLeftovers] = NULL;

    for (av = con->leftovers + con->nextLeftover; *av != NULL; av++) {
	if ((rc = poptBitsAdd(*ap, *av)) != 0)
	    break;
    }
/*@-nullstate@*/
    return rc;
/*@=nullstate@*/
}

int poptSaveBits(poptBits * bitsp,
		/*@unused@*/ UNUSED(unsigned int argInfo), const char * s)
{
    char *tbuf = NULL;
    char *t, *te;
    int rc = 0;

    if (bitsp == NULL || s == NULL || *s == '\0' || _poptBitsNew(bitsp))
	return POPT_ERROR_NULLARG;

    /* Parse comma separated attributes. */
    te = tbuf = xstrdup(s);
    assert(te);
    while ((t = te) != NULL && *t) {
	while (*te != '\0' && *te != ',')
	    te++;
	if (*te != '\0')
	    *te++ = '\0';
	/* XXX Ignore empty strings. */
	if (*t == '\0')
	    continue;
	/* XXX Permit negated attributes. caveat emptor: false negatives. */
	if (*t == '!') {
	    t++;
	    if ((rc = poptBitsChk(*bitsp, t)) > 0)
		rc = poptBitsDel(*bitsp, t);
	} else
	    rc = poptBitsAdd(*bitsp, t);
	if (rc)
	    break;
    }
    tbuf = _free(tbuf);
    return rc;
}
/*@=sizeoftype@*/

int poptSaveString(const char *** argvp,
		/*@unused@*/ UNUSED(unsigned int argInfo), const char * val)
{
    int argc = 0;

    if (argvp == NULL || val == NULL)
	return POPT_ERROR_NULLARG;

    /* XXX likely needs an upper bound on argc. */
    if (*argvp != NULL)
    while ((*argvp)[argc] != NULL)
	argc++;

/*@-unqualifiedtrans -nullstate@*/	/* XXX no annotation for (*argvp) */
    if ((*argvp = (const char**) xrealloc(*argvp, (argc + 1 + 1) * sizeof(**argvp))) != NULL) {
	(*argvp)[argc++] = xstrdup(val);
	(*argvp)[argc  ] = NULL;
    }
    return 0;
/*@=unqualifiedtrans =nullstate@*/
}

/*@unchecked@*/
static unsigned seed = 0;

typedef int64_t * poptStack_t;

static long long poptCalculator(long long arg0, unsigned argInfo, long long arg1,
		/*@null@*/ const char * expr, int * rcp)
{
int ixmax = 20;	/* XXX overkill */
poptStack_t stk = (poptStack_t) memset((poptStack_t)alloca(ixmax*sizeof(*stk)), 0, (ixmax*sizeof(*stk)));
    int ix = 0;
size_t nt = 64;	/* XXX overkill */
char * t = (char*) alloca(nt);
char * te = t;
const char * s;
    int rc = 0;		/* assume success */
    long long retval = 0;
const char ** av = NULL;
int ac = 0;
int xx;

    stk[ix++] = arg0;

    if (arg1 != 0 && LF_ISSET(RANDOM)) {
#if defined(HAVE_SRANDOM)
	if (!seed) {
	    srandom((unsigned)getpid());
	    srandom((unsigned)random());
	}
	arg1 = random() % (arg1 > 0 ? arg1 : -arg1);
	arg1++;
#else
	/* XXX avoid adding POPT_ERROR_UNIMPLEMENTED to minimize i18n churn. */
	rc = POPT_ERROR_BADOPERATION;
	goto exit;
#endif
    }
    if (!LF_ISSET(CALCULATOR) && LF_ISSET(NOT))
	arg1 = ~arg1;

    stk[ix++] = arg1;

    /* XXX hotwire a RPN program. */
    if (expr == NULL) {
    switch (LF_ISSET(LOGICALOPS)) {
	case POPT_ARGFLAG_OR:	*te++ = '|';	break;
	case POPT_ARGFLAG_AND:	*te++ = '&';	break;
	case POPT_ARGFLAG_XOR:	*te++ = '^';	break;
    default:
	if (LF_ISSET(CALCULATOR))	/* XXX hotwire +/- operations. */
		*te++ = LF_ISSET(NOT) ? '-' : '+';
	else if (!LF_ISSET(LOGICALOPS))
		*te++ = '=';
	else {
	    rc = POPT_ERROR_BADOPERATION;
	    goto exit;
	}
	break;
    }
	*te = '\0';
    }

s = (expr ? expr : t);
xx = poptParseArgvString(s, &ac, &av);	/* XXX split on CSV character set. */
assert(!xx && av);

    if (av) {
    int i;
    for (i = 0; av[i] != NULL; i++) {
	const char * arg = av[i];
	size_t len = strlen(arg);
	int c = (int) *arg;

	if (len == 0)
	    continue;
	if (len > 1) {
	    char * end = NULL;
	    if (ix >= ixmax) {
		rc = POPT_ERROR_STACKOVERFLOW;
		goto exit;
	    }
	    stk[ix++] = strtoll(arg, &end, 0);
	    if (end && *end != '\0') {
		rc = POPT_ERROR_BADNUMBER;
		goto exit;
	    }
	} else {
	    if (ix-- < 2) {
		rc = POPT_ERROR_STACKUNDERFLOW;
		goto exit;
	    }
	    switch (c) {
	    case 'd':		/* duplicate */
		ix++;
		stk[ix] = stk[ix-1];
		ix++;
		break;		/* XXX FIXME: initial arg0 cannot be dupe'd. */
	    case 'P':		/* pop */
		break;		/* XXX FIXME: initial arg0 cannot be pop'd. */
	    case 'r':		/* reverse */
		ix++;
		stk[ix] = stk[ix-2];
		stk[ix-2] = stk[ix-1];
		stk[ix-1] = stk[ix];
		break;
	    case '=':	stk[ix-1]  = stk[ix];	break;
	    case '|':	stk[ix-1] |= stk[ix];	break;
	    case '&':	stk[ix-1] &= stk[ix];	break;
	    case '^':	stk[ix-1] ^= stk[ix];	break;
	    case '+':	stk[ix-1] += stk[ix];	break;
	    case '-':	stk[ix-1] -= stk[ix];	break;
	    case '*':	stk[ix-1] *= stk[ix];	break;
	    case '/':
	    case '%':
		if (stk[ix] == 0) {
		    rc = POPT_ERROR_BADOPERATION; /* XXX POPT_ERROR_DIVZERO */
		    goto exit;
		}
		if (c == (int)'%')
		    stk[ix-1] %= stk[ix];
		else
		    stk[ix-1] /= stk[ix];
		break;
	    default:
		rc = POPT_ERROR_BADOPERATION;
		goto exit;
		/*@notreached@*/ break;
	    }
	}
    }
   }

    if (ix-- < 1) {
	rc = POPT_ERROR_STACKUNDERFLOW;
	goto exit;
    }
    retval = stk[ix];

exit:
    if (av)
	av = _free(av);
    *rcp = rc;
    return retval;
}

int poptSaveLongLong(long long * arg, unsigned int argInfo, long long aLongLong)
{
    long long retval = 0;
    int rc = 0;

    if (arg == NULL
#ifdef	NOTYET
    /* XXX Check alignment, may fail on funky platforms. */
     || (((unsigned long)arg) & (sizeof(*arg)-1))
#endif
    )
	return POPT_ERROR_NULLARG;

    retval = poptCalculator(*arg, argInfo, (long long)aLongLong, NULL, &rc);
    if (!rc)
	*arg = (long long) retval;

    return rc;
}

int poptSaveLong(long * arg, unsigned int argInfo, long aLong)
{
    long long retval = 0;
    int rc = 0;

    /* XXX Check alignment, may fail on funky platforms. */
    if (arg == NULL || (((unsigned long)arg) & (sizeof(*arg)-1)))
	return POPT_ERROR_NULLARG;

    retval = poptCalculator(*arg, argInfo, (long long)aLong, NULL, &rc);
    if (!rc)
	*arg = (long) retval;

    return rc;
}

int poptSaveInt(/*@null@*/ int * arg, unsigned int argInfo, long aLong)
{
    long long retval = 0;
    int rc = 0;

    /* XXX Check alignment, may fail on funky platforms. */
    if (arg == NULL || (((unsigned long)arg) & (sizeof(*arg)-1)))
	return POPT_ERROR_NULLARG;

    retval = poptCalculator(*arg, argInfo, (long long)aLong, NULL, &rc);
    if (!rc)
	*arg = (int) retval;

    return rc;
}

int poptSaveShort(/*@null@*/ short * arg, unsigned int argInfo, long aLong)
{
    long long retval = 0;
    int rc = 0;

    /* XXX Check alignment, may fail on funky platforms. */
    if (arg == NULL || (((unsigned long)arg) & (sizeof(*arg)-1)))
	return POPT_ERROR_NULLARG;

    retval = poptCalculator(*arg, argInfo, (long long)aLong, NULL, &rc);
    if (!rc)
	*arg = (short) retval;

    return rc;
}

/**
 * Return argInfo field, handling POPT_ARGFLAG_TOGGLE overrides.
 * @param con		context
 * @param *opt           option
 * @return		argInfo
 */
static unsigned int poptArgInfo(poptContext con, const struct poptOption * opt)
	/*@*/
{
    unsigned int argInfo = opt->argInfo;

    if (con->os->argv != NULL && con->os->next > 0 && opt->longName != NULL)
    if (LF_ISSET(TOGGLE)) {
	const char * longName = con->os->argv[con->os->next-1];
	while (*longName == '-') longName++;
	/* XXX almost good enough but consider --[no]nofoo corner cases. */
	if (longName[0] != opt->longName[0] || longName[1] != opt->longName[1])
	{
	    if (!LF_ISSET(XOR)) {	/* XXX dont toggle with XOR */
		/* Toggle POPT_BIT_SET <=> POPT_BIT_CLR. */
		if (LF_ISSET(LOGICALOPS))
		    argInfo ^= (POPT_ARGFLAG_OR|POPT_ARGFLAG_AND);
		argInfo ^= POPT_ARGFLAG_NOT;
	    }
	}
    }
    return argInfo;
}

/**
 * Parse an integer expression.
 * @retval *llp		integer expression value
 * @param argInfo	integer expression type
 * @param val		integer expression string
 * @return		0 on success, otherwise POPT_* error.
 */
static int poptParseInteger(long long * llp,
		/*@unused@*/ UNUSED(unsigned int argInfo),
		/*@null@*/ const char * val)
	/*@modifies *llp @*/
{
    if (val) {
	char *end = NULL;
	*llp = strtoll(val, &end, 0);

	/* XXX parse scaling suffixes here. */

	if (!(end && *end == '\0'))
	    return POPT_ERROR_BADNUMBER;
    } else
	*llp = 0;
    return 0;
}

/**
 * Save the option argument through the (*opt->arg) pointer.
 * @param con		context
 * @param opt           option
 * @return		0 on success, otherwise POPT_* error.
 */
static int poptSaveArg(poptContext con, const struct poptOption * opt)
	/*@globals fileSystem, internalState @*/
	/*@modifies con, fileSystem, internalState @*/
{
    int rc = 0;		/* assume success */
    poptArg arg;
    arg.ptr = opt->arg;

    switch (poptArgType(opt)) {
    case POPT_ARG_BITSET:
	/* XXX memory leak, application is responsible for free. */
	rc = poptSaveBits(arg.ptr, opt->argInfo, con->os->nextArg);
	/*@switchbreak@*/ break;
    case POPT_ARG_ARGV:
	/* XXX memory leak, application is responsible for free. */
	rc = poptSaveString(arg.ptr, opt->argInfo, con->os->nextArg);
	/*@switchbreak@*/ break;
    case POPT_ARG_STRING:
	/* XXX memory leak, application is responsible for free. */
	arg.argv[0] = (con->os->nextArg) ? xstrdup(con->os->nextArg) : NULL;
	/*@switchbreak@*/ break;

    case POPT_ARG_LONGLONG:
    case POPT_ARG_LONG:
    case POPT_ARG_INT:
    case POPT_ARG_SHORT:
    case POPT_ARG_NONE:
    case POPT_ARG_VAL:
    {	unsigned argInfo = poptArgInfo(con, opt);
	long long aNUM = 0;
	const char * expr = LF_ISSET(CALCULATOR) ? opt->argDescrip : NULL;

	if (poptArgType(opt) == POPT_ARG_NONE)
	    aNUM = 1LL;
	else if (poptArgType(opt) == POPT_ARG_VAL)
	    aNUM = (long long) opt->val;
	else if ((rc = poptParseInteger(&aNUM, argInfo, con->os->nextArg)) != 0)
	    break;

	switch (poptArgType(opt)) {
	case POPT_ARG_LONGLONG:
/* XXX let's not demand C99 compiler flags for <limits.h> quite yet. */
#if !defined(LLONG_MAX)
#   define LLONG_MAX    9223372036854775807LL
#   define LLONG_MIN    (-LLONG_MAX - 1LL)
#endif
	    if (aNUM == LLONG_MIN || aNUM == LLONG_MAX) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    /* XXX pointer alignment check? */
	    aNUM = poptCalculator(arg.longlongp[0], argInfo, aNUM, expr, &rc);
	    if (!rc)
		arg.longlongp[0] = (long long) aNUM;
	    /*@innerbreak@*/ break;
	case POPT_ARG_LONG:
	    if (aNUM < (long long)LONG_MIN || aNUM > (long long)LONG_MAX) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    /* XXX pointer alignment check? */
	    aNUM = poptCalculator(arg.longp[0], argInfo, aNUM, expr, &rc);
	    if (!rc)
		arg.longp[0] = (long) aNUM;
	    /*@innerbreak@*/ break;
	case POPT_ARG_INT:
	    if (aNUM < (long long)INT_MIN || aNUM > (long long)INT_MAX) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    /*@fallthrough@*/
	case POPT_ARG_NONE:
	case POPT_ARG_VAL:
	    /* XXX pointer alignment check? */
	    aNUM = poptCalculator(arg.intp[0], argInfo, aNUM, expr, &rc);
	    if (!rc)
		arg.intp[0] = (int) aNUM;
	    /*@innerbreak@*/ break;
	case POPT_ARG_SHORT:
	    if (aNUM < (long long)SHRT_MIN || aNUM > (long long)SHRT_MAX) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    /* XXX pointer alignment check? */
	    aNUM = poptCalculator(arg.shortp[0], argInfo, aNUM, expr, &rc);
	    if (!rc)
		arg.shortp[0] = (short) aNUM;
	    /*@innerbreak@*/ break;
	}
    }   /*@switchbreak@*/ break;

    case POPT_ARG_FLOAT:
    case POPT_ARG_DOUBLE:
    {	char *end = NULL;
	double aDouble = 0.0;

	if (con->os->nextArg) {
/*@-mods@*/
	    int saveerrno = errno;
	    errno = 0;
	    aDouble = strtod(con->os->nextArg, &end);
	    if (errno == ERANGE) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    errno = saveerrno;
/*@=mods@*/
	    if (*end != '\0') {
		rc = POPT_ERROR_BADNUMBER;
		goto exit;
	    }
	}

	switch (poptArgType(opt)) {
	case POPT_ARG_DOUBLE:
	    arg.doublep[0] = aDouble;
	    /*@innerbreak@*/ break;
	case POPT_ARG_FLOAT:
#if !defined(DBL_EPSILON) && !defined(__LCLINT__)
#define DBL_EPSILON 2.2204460492503131e-16
#endif
#define POPT_ABS(a)	((((a) - 0.0) < DBL_EPSILON) ? -(a) : (a))
	    if ((FLT_MIN - POPT_ABS(aDouble)) > DBL_EPSILON
	     || (POPT_ABS(aDouble) - FLT_MAX) > DBL_EPSILON) {
		rc = POPT_ERROR_OVERFLOW;
		goto exit;
	    }
	    arg.floatp[0] = (float) aDouble;
	    /*@innerbreak@*/ break;
	}
    }   /*@switchbreak@*/ break;
    case POPT_ARG_MAINCALL:
/*@-assignexpose -type@*/
	con->maincall = opt->arg;
/*@=assignexpose =type@*/
	/*@switchbreak@*/ break;
    default:
	fprintf(stdout, POPT_("option type (%u) not implemented in popt\n"),
		poptArgType(opt));
	exit(EXIT_FAILURE);
	/*@notreached@*/ /*@switchbreak@*/ break;
    }

exit:
    return rc;
}

/* returns 'val' element, -1 on last item, POPT_ERROR_* on error */
int poptGetNextOpt(poptContext con)
{
    const struct poptOption * opt = NULL;
    int done = 0;
    int rc = -1;

    if (con == NULL)
	goto exit;

    while (!done) {
	poptCallbackType cb = NULL;
	const void * cbData = NULL;
	const char * longArg = NULL;
	int canstrip = 0;
	int shorty = 0;

	while (!con->os->nextCharArg && con->os->next == con->os->argc
		&& con->os > con->optionStack) {
	    cleanOSE(con->os--);
	}

	if (!con->os->nextCharArg && con->os->next == con->os->argc) {
	    invokeCallbacksPOST(con, con->options);

	    if (con->maincall) {
		/*@-noeffectuncon @*/
		(void) (*con->maincall) (con->ac, con->av);
		/*@=noeffectuncon @*/
		rc = -1;
		goto exit;
	    }

	    rc = (con->doExec) ? execCommand(con) : -1;
	    goto exit;
	}

	/* Process next long option */
	if (!con->os->nextCharArg) {
	    const char * origOptString = NULL;
	    const char * optString;
	    int thisopt;

/*@-sizeoftype@*/
	    if (con->os->argb && PBM_ISSET(con->os->next, con->os->argb)) {
		con->os->next++;
		continue;
	    }
/*@=sizeoftype@*/
	    thisopt = con->os->next;
assert(con->os->argv);	/* XXX can't happen */
	    if (con->os->argv != NULL)
	    origOptString = con->os->argv[con->os->next++];

assert(origOptString);	/* XXX can't happen */
	    if (origOptString == NULL) {
		rc = POPT_ERROR_BADOPT;
		goto exit;
	    }

	    if (con->restLeftover || *origOptString != '-' ||
		(*origOptString == '-' && origOptString[1] == '\0'))
	    {
		if (con->flags & POPT_CONTEXT_POSIXMEHARDER)
		    con->restLeftover = 1;
		if (con->flags & POPT_CONTEXT_ARG_OPTS) {
		    con->os->nextArg = xstrdup(origOptString);
		    rc = 0;
		    goto exit;
		}
assert(con->leftovers);		/* XXX can't happen */
		if (con->leftovers != NULL)
		    con->leftovers[con->numLeftovers++] = origOptString;
		continue;
	    }

	    /* Make a copy we can hack at */
	    optString = origOptString;

	    if (optString[0] == '\0') {
		rc = POPT_ERROR_BADOPT;
		goto exit;
	    }

	    if (optString[1] == '-' && !optString[2]) {
		con->restLeftover = 1;
		continue;
	    } else {
		unsigned int argInfo = 0;
		const char *oe;
                size_t optStringLen;

		optString++;
		if (*optString == '-')
		    optString++;
		else
		    argInfo |= POPT_ARGFLAG_ONEDASH;

		/* Check for "--long=arg" option. */
		for (oe = optString; *oe && *oe != '='; oe++)
		    {};
		optStringLen = (size_t)(oe - optString);
		if (*oe == '=')
		    longArg = oe + 1;

		/* XXX aliases with arg substitution need "--alias=arg" */
		if (handleAlias(con, optString, optStringLen, '\0', longArg)) {
		    longArg = NULL;
		    continue;
		}

		if (handleExec(con, optString, '\0'))
		    continue;

		opt = findOption(con->options, optString, optStringLen,
				'\0', &cb, &cbData, argInfo);
		if (!opt && !LF_ISSET(ONEDASH)) {
		    rc = POPT_ERROR_BADOPT;
		    goto exit;
		}
	    }

	    if (!opt) {
		con->os->nextCharArg = origOptString + 1;
		longArg = NULL;
	    } else {
		if (con->os == con->optionStack && F_ISSET(opt, STRIP)) {
		    canstrip = 1;
		    poptStripArg(con, thisopt);
		}
		shorty = 0;
	    }
	}

	/* Process next short option */
	if (con->os->nextCharArg) {
	    const char * nextCharArg = con->os->nextCharArg;

	    con->os->nextCharArg = NULL;

	    if (handleAlias(con, NULL, 0, *nextCharArg, nextCharArg + 1))
		continue;

	    if (handleExec(con, NULL, *nextCharArg)) {
		/* Restore rest of short options for further processing */
		nextCharArg++;
		if (*nextCharArg != '\0')
		    con->os->nextCharArg = nextCharArg;
		continue;
	    }

	    opt = findOption(con->options, NULL, 0,
				*nextCharArg, &cb, &cbData, 0);
	    if (!opt) {
		rc = POPT_ERROR_BADOPT;
		goto exit;
	    }
	    shorty = 1;

	    nextCharArg++;
	    if (*nextCharArg != '\0')
		con->os->nextCharArg = nextCharArg + (int)(*nextCharArg == '=');
	}

assert(opt != NULL);	/* XXX can't happen */
	if (opt == NULL) {
	    rc = POPT_ERROR_BADOPT;
	    goto exit;
	}

	rc = 0;		/* assume success */
	switch (poptArgType(opt)) {
	case POPT_ARG_NONE:
	case POPT_ARG_VAL:
	    if (longArg
	     || (con->os->nextCharArg && con->os->nextCharArg[0] == '='))
		rc = POPT_ERROR_UNWANTEDARG;
	    break;
	default:
	    con->os->nextArg = _free(con->os->nextArg);
	    if (longArg) {
		longArg = expandNextArg(con, longArg);
		con->os->nextArg = (char *) longArg;
	    } else if (con->os->nextCharArg) {
		int skip = (con->os->nextCharArg[0] == '=');
		longArg = expandNextArg(con, con->os->nextCharArg + skip);
		con->os->nextArg = (char *) longArg;
		con->os->nextCharArg = NULL;
	    } else {
		while (con->os->next == con->os->argc &&
			con->os > con->optionStack)
		{
		    cleanOSE(con->os--);
		}
		if (con->os->next == con->os->argc) {
		    if (!F_ISSET(opt, OPTIONAL)) {
			rc = POPT_ERROR_NOARG;
			break;
		    }
		    con->os->nextArg = NULL;
		} else {
		    /* Avoid short args and alias expansions. */
		    if (con->os == con->optionStack
		     && F_ISSET(opt, STRIP) && canstrip)
			poptStripArg(con, con->os->next);

assert(con->os->argv);	/* XXX can't happen */
		    if (con->os->argv != NULL) {
			if (F_ISSET(opt, OPTIONAL) &&
			    con->os->argv[con->os->next][0] == '-') {
			    con->os->nextArg = NULL;
			} else {
			    /* XXX watchout: subtle side-effects live here. */
			    longArg = con->os->argv[con->os->next++];
			    longArg = expandNextArg(con, longArg);
			    con->os->nextArg = (char *) longArg;
			}
		    }
		}
	    }
	    break;
	}
	longArg = NULL;
	if (rc || (opt->arg && (rc = poptSaveArg(con, opt)) != 0))
	    goto exit;

	if (cb)
	    invokeCallbacksOPTION(con, con->options, opt, cbData, shorty);
	else if (opt->val && (poptArgType(opt) != POPT_ARG_VAL))
	    done = 1;

	if ((con->ac + 2) >= (con->nav)) {
	    con->nav += 10;
	    con->av = (poptArgv) xrealloc(con->av,
			    sizeof(*con->av) * con->nav);
	}

assert(con->av);
	if (con->av) {
	    size_t nb = (opt->longName ? strlen(opt->longName) : 0) + sizeof("--");
	    char *s = (char*) xmalloc(nb);
assert(s);	/* XXX can't happen */
	    if (s != NULL) {
		con->av[con->ac++] = s;
		*s++ = '-';
		if (opt->longName) {
		    if (!F_ISSET(opt, ONEDASH))
			*s++ = '-';
		    s = stpcpy(s, opt->longName);
		} else {
		    *s++ = opt->shortName;
		    *s = '\0';
		}
	    } else
		con->av[con->ac++] = NULL;
	}

	switch (poptArgType(opt)) {
	case POPT_ARG_NONE:
	case POPT_ARG_VAL:
	    break;
	default:
	    if (con->os->nextArg)
	        con->av[con->ac++] = xstrdup(con->os->nextArg);
	    break;
	}

    }
assert(opt);	/* XXX can't happen */
    rc = (opt ? opt->val : -1);

exit:
    return rc;
}

char * poptGetOptArg(poptContext con)
{
    char * ret = NULL;
    if (con) {
	ret = con->os->nextArg;
	con->os->nextArg = NULL;
    }
    return ret;
}

const char * poptGetArg(poptContext con)
{
    const char * ret = NULL;
    if (con && con->leftovers != NULL && con->nextLeftover < con->numLeftovers)
	ret = con->leftovers[con->nextLeftover++];
    return ret;
}

const char * poptPeekArg(poptContext con)
{
    const char * ret = NULL;
    if (con && con->leftovers != NULL && con->nextLeftover < con->numLeftovers)
	ret = con->leftovers[con->nextLeftover];
    return ret;
}

const char ** poptGetArgs(poptContext con)
{
    if (!(con && con->leftovers && con->numLeftovers != con->nextLeftover))
	return NULL;

    /* some apps like [like RPM ] need this NULL terminated */
    con->leftovers[con->numLeftovers] = NULL;

/*@-nullret -nullstate @*/	/* FIX: typedef double indirection. */
    return (con->leftovers + con->nextLeftover);
/*@=nullret =nullstate @*/
}

static /*@null@*/
poptItem poptFreeItems(/*@only@*/ /*@null@*/ poptItem items, int nitems)
	/*@modifies items @*/
{
    if (items != NULL) {
	poptItem item = items;
	while (--nitems >= 0) {
/*@-modobserver -observertrans -dependenttrans@*/
	    item->option.longName = _free(item->option.longName);
	    item->option.descrip = _free(item->option.descrip);
	    item->option.argDescrip = _free(item->option.argDescrip);
/*@=modobserver =observertrans =dependenttrans@*/
	    item->argv = _free(item->argv);
	    item++;
	}
	items = _free(items);
    }
    return NULL;
}

poptContext poptFreeContext(poptContext con)
{
    if (con == NULL) return con;
    poptResetContext(con);
    con->os->argb = _free(con->os->argb);

    con->aliases = poptFreeItems(con->aliases, con->numAliases);
    con->numAliases = 0;

    con->execs = poptFreeItems(con->execs, con->numExecs);
    con->numExecs = 0;

    con->leftovers = _free(con->leftovers);
    con->av = _free(con->av);
    con->appName = _free(con->appName);
    con->otherHelp = _free(con->otherHelp);
    con->execPath = _free(con->execPath);
    con->arg_strip = PBM_FREE(con->arg_strip);

    con = _free(con);
    return con;
}

int poptAddAlias(poptContext con, struct poptAlias alias,
		/*@unused@*/ UNUSED(int flags))
{
    struct poptItem_s item_buf;
    poptItem item = &item_buf;
    memset(item, 0, sizeof(*item));
    item->option.longName = alias.longName;
    item->option.shortName = alias.shortName;
    item->option.argInfo = POPT_ARGFLAG_DOC_HIDDEN;
    item->option.arg = 0;
    item->option.val = 0;
    item->option.descrip = NULL;
    item->option.argDescrip = NULL;
    item->argc = alias.argc;
    item->argv = alias.argv;
    return poptAddItem(con, item, 0);
}

int poptAddItem(poptContext con, poptItem newItem, int flags)
{
    poptItem * items, item;
    size_t * nitems;
    int    * naliases;

    switch (flags) {
    case 1:
	items = &con->execs;
	nitems = &con->numExecs;
      *items = (poptItem) xrealloc((*items), ((*nitems) + 1) * sizeof(**items));
	break;
    case 0:
	items = &con->aliases;
	naliases = &con->numAliases;
      *items = (poptItem) xrealloc((*items), ((*naliases) + 1) * sizeof(**items));
	break;
    default:
	return 1;
	/*@notreached@*/ break;
    }

assert(*items);	/* XXX can't happen */
    if ((*items) == NULL)
	return 1;

    item =(flags ? (*items) + (*nitems) : (*items) + (*naliases) );

    item->option.longName =
	(newItem->option.longName ? xstrdup(newItem->option.longName) : NULL);
    item->option.shortName = newItem->option.shortName;
    item->option.argInfo = newItem->option.argInfo;
    item->option.arg = newItem->option.arg;
    item->option.val = newItem->option.val;
    item->option.descrip =
	(newItem->option.descrip ? xstrdup(newItem->option.descrip) : NULL);
    item->option.argDescrip =
       (newItem->option.argDescrip ? xstrdup(newItem->option.argDescrip) : NULL);
    item->argc = newItem->argc;
    item->argv = newItem->argv;

    (flags ? (*nitems)++ : (*naliases)++ );

    return 0;
}

const char * poptBadOption(poptContext con, unsigned int flags)
{
    struct optionStackEntry * os = NULL;

    if (con != NULL)
	os = (flags & POPT_BADOPTION_NOALIAS) ? con->optionStack : con->os;

    return (os != NULL && os->argv != NULL ? os->argv[os->next - 1] : NULL);
}

const char * poptStrerror(const int error)
{
    switch (error) {
      case POPT_ERROR_NOARG:
	return POPT_("missing argument");
      case POPT_ERROR_BADOPT:
	return POPT_("unknown option");
      case POPT_ERROR_BADOPERATION:
	return POPT_("mutually exclusive logical operations requested");
      case POPT_ERROR_NULLARG:
	return POPT_("opt->arg should not be NULL");
      case POPT_ERROR_OPTSTOODEEP:
	return POPT_("aliases nested too deeply");
      case POPT_ERROR_BADQUOTE:
	return POPT_("error in parameter quoting");
      case POPT_ERROR_BADNUMBER:
	return POPT_("invalid numeric value");
      case POPT_ERROR_OVERFLOW:
	return POPT_("number too large or too small");
      case POPT_ERROR_MALLOC:
	return POPT_("memory allocation failed");
      case POPT_ERROR_BADCONFIG:
	return POPT_("config file failed sanity test");
      case POPT_ERROR_UNWANTEDARG:
	return POPT_("option does not take an argument");
      case POPT_ERROR_STACKUNDERFLOW:
	return POPT_("stack underflow");
      case POPT_ERROR_STACKOVERFLOW:
	return POPT_("stack overflow");
      case POPT_ERROR_ERRNO:
	return strerror(errno);
      default:
	return POPT_("unknown error");
    }
}

int poptStuffArgs(poptContext con, const char ** argv)
{
    int argc;
    int rc;

    if ((con->os - con->optionStack) == con->optionDepth)
	return POPT_ERROR_OPTSTOODEEP;

    for (argc = 0; argv[argc]; argc++)
	{};

    con->os++;
    con->os->next = 0;
    con->os->nextArg = NULL;
    con->os->nextCharArg = NULL;
    con->os->currAlias = NULL;
    rc = poptDupArgv(argc, argv, &con->os->argc, &con->os->argv);
    con->os->argb = NULL;
    con->os->stuffed = 1;

    return rc;
}

const char * poptGetInvocationName(poptContext con)
{
    return (con->os->argv ? con->os->argv[0] : "");
}

int poptStrippedArgv(poptContext con, int argc, char ** argv)
{
    int numargs = argc;
    int j = 1;
    int i;

/*@-sizeoftype@*/
    if (con->arg_strip)
    for (i = 1; i < argc; i++) {
	if (PBM_ISSET(i, con->arg_strip))
	    numargs--;
    }

    for (i = 1; i < argc; i++) {
	if (con->arg_strip && PBM_ISSET(i, con->arg_strip))
	    continue;
	argv[j] = (j < numargs) ? argv[i] : NULL;
	j++;
    }
/*@=sizeoftype@*/

    return numargs;
}
