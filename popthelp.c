/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */

/** \ingroup popt
 * \file popt/popthelp.c
 */

/* (C) 1998-2002 Red Hat, Inc. -- Licensing details are in the COPYING
   file accompanying popt source distributions, available from 
   ftp://ftp.rpm.org/pub/rpm/dist. */

#include "system.h"

#define	POPT_USE_TIOCGWINSZ
#ifdef	POPT_USE_TIOCGWINSZ
#include <sys/ioctl.h>
#endif

#define	POPT_WCHAR_HACK
#ifdef 	POPT_WCHAR_HACK
#include <wchar.h>			/* for mbsrtowcs */
/*@access mbstate_t @*/
#endif

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "poptint.h"

/*@access poptContext@*/

/**
 * Display arguments.
 * @param con		context
 * @param foo		(unused)
 * @param key		option(s)
 * @param arg		(unused)
 * @param data		(unused)
 */
/*@exits@*/
static void displayArgs(poptContext con,
		/*@unused@*/ enum poptCallbackReason foo,
		struct poptOption * key, 
		/*@unused@*/ const char * arg, /*@unused@*/ void * data)
	/*@globals fileSystem@*/
	/*@modifies con, fileSystem@*/
{
    if (key->shortName == '?')
	poptPrintHelp(con, stdout, 0);
    else
	poptPrintUsage(con, stdout, 0);
/*@i@*/ con = poptFreeContext(con);		/* XXX keep valgrind happy */
    exit(0);
}

#ifdef	NOTYET
/*@unchecked@*/
static int show_option_defaults = 0;
#endif

/**
 * Empty table marker to enable displaying popt alias/exec options.
 */
/*@observer@*/ /*@unchecked@*/
struct poptOption poptAliasOptions[] = {
    POPT_TABLEEND
};

/**
 * Auto help table options.
 */
/*@-castfcnptr@*/
/*@observer@*/ /*@unchecked@*/
struct poptOption poptHelpOptions[] = {
  { NULL, '\0', POPT_ARG_CALLBACK, (void *)displayArgs, 0, NULL, NULL },
  { "help", '?', 0, NULL, (int)'?', N_("Show this help message"), NULL },
  { "usage", '\0', 0, NULL, (int)'u', N_("Display brief usage message"), NULL },
    POPT_TABLEEND
} ;

/*@observer@*/ /*@unchecked@*/
static struct poptOption poptHelpOptions2[] = {
/*@-readonlytrans@*/
  { NULL, '\0', POPT_ARG_INTL_DOMAIN, PACKAGE, 0, NULL, NULL},
/*@=readonlytrans@*/
  { NULL, '\0', POPT_ARG_CALLBACK, (void *)displayArgs, 0, NULL, NULL },
  { "help", '?', 0, NULL, (int)'?', N_("Show this help message"), NULL },
  { "usage", '\0', 0, NULL, (int)'u', N_("Display brief usage message"), NULL },
#ifdef	NOTYET
  { "defaults", '\0', POPT_ARG_NONE, &show_option_defaults, 0,
	N_("Display option defaults in message"), NULL },
#endif
    POPT_TABLEEND
} ;

/*@observer@*/ /*@unchecked@*/
struct poptOption * poptHelpOptionsI18N = poptHelpOptions2;
/*@=castfcnptr@*/

#define	_POPTHELP_MAXLINE	((size_t)79)

typedef struct columns_s {
    size_t cur;
    size_t max;
} * columns_t;

/**
 * Return no. of columns in output window.
 * @param fp		FILE
 * @return		no. of columns
 */
static size_t maxColumnWidth(FILE *fp)
	/*@*/
{
    size_t maxcols = _POPTHELP_MAXLINE;
#if defined(TIOCGWINSZ)
    struct winsize ws;
    int fdno = fileno(fp ? fp : stdout);

    if (fdno >= 0 && !ioctl(fdno, TIOCGWINSZ, &ws)
     && ws.ws_col > maxcols && ws.ws_col < 256)
	maxcols = ws.ws_col - 1;
#endif
    return maxcols;
}

/**
 * @param table		option(s)
 */
/*@observer@*/ /*@null@*/ static const char *
getTableTranslationDomain(/*@null@*/ const struct poptOption *table)
	/*@*/
{
    const struct poptOption *opt;

    if (table != NULL)
    for (opt = table; opt->longName || opt->shortName || opt->arg; opt++) {
	if (opt->argInfo == POPT_ARG_INTL_DOMAIN)
	    return opt->arg;
    }
    return NULL;
}

/**
 * @param opt		option(s)
 * @param translation_domain	translation domain
 */
/*@observer@*/ /*@null@*/ static const char *
getArgDescrip(const struct poptOption * opt,
		/*@-paramuse@*/ /* FIX: i18n macros disabled with lclint */
		/*@null@*/ const char * translation_domain)
		/*@=paramuse@*/
	/*@*/
{
    if (!(opt->argInfo & POPT_ARG_MASK)) return NULL;

    if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_MAINCALL)
	return opt->argDescrip;

    if (opt->argDescrip) {
	/* Some strings need popt library, not application, i18n domain. */
	if (opt == (poptHelpOptions + 1)
	 || opt == (poptHelpOptions + 2)
	 || !strcmp(opt->argDescrip,N_("Help options:"))
	 || !strcmp(opt->argDescrip,N_("Options implemented via popt alias/exec:")))
	    return POPT_(opt->argDescrip);

	/* Use the application i18n domain. */
	return D_(translation_domain, opt->argDescrip);
    }

    switch (opt->argInfo & POPT_ARG_MASK) {
    case POPT_ARG_NONE:		return POPT_("NONE");
#ifdef	DYING
    case POPT_ARG_VAL:		return POPT_("VAL");
#else
    case POPT_ARG_VAL:		return NULL;
#endif
    case POPT_ARG_INT:		return POPT_("INT");
    case POPT_ARG_LONG:		return POPT_("LONG");
    case POPT_ARG_STRING:	return POPT_("STRING");
    case POPT_ARG_FLOAT:	return POPT_("FLOAT");
    case POPT_ARG_DOUBLE:	return POPT_("DOUBLE");
    case POPT_ARG_MAINCALL:	return NULL;
    default:			return POPT_("ARG");
    }
}

/**
 * Display default value for an option.
 * @param lineLength	display positions remaining
 * @param opt		option(s)
 * @param translation_domain	translation domain
 * @return
 */
static /*@only@*/ /*@null@*/ char *
singleOptionDefaultValue(size_t lineLength,
		const struct poptOption * opt,
		/*@-paramuse@*/ /* FIX: i18n macros disabled with lclint */
		/*@null@*/ const char * translation_domain)
		/*@=paramuse@*/
	/*@*/
{
    const char * defstr = D_(translation_domain, "default");
    char * le = malloc(4*lineLength + 1);
    char * l = le;

    if (le == NULL) return NULL;	/* XXX can't happen */
/*@-boundswrite@*/
    *le = '\0';
    *le++ = '(';
    strcpy(le, defstr);	le += strlen(le);
    *le++ = ':';
    *le++ = ' ';
    if (opt->arg)	/* XXX programmer error */
    switch (opt->argInfo & POPT_ARG_MASK) {
    case POPT_ARG_VAL:
    case POPT_ARG_INT:
    {	long aLong = *((int *)opt->arg);
	le += sprintf(le, "%ld", aLong);
    }	break;
    case POPT_ARG_LONG:
    {	long aLong = *((long *)opt->arg);
	le += sprintf(le, "%ld", aLong);
    }	break;
    case POPT_ARG_FLOAT:
    {	double aDouble = *((float *)opt->arg);
	le += sprintf(le, "%g", aDouble);
    }	break;
    case POPT_ARG_DOUBLE:
    {	double aDouble = *((double *)opt->arg);
	le += sprintf(le, "%g", aDouble);
    }	break;
    case POPT_ARG_MAINCALL:
	le += sprintf(le, "%p", opt->arg);
	break;
    case POPT_ARG_STRING:
    {	const char * s = *(const char **)opt->arg;
	if (s == NULL) {
	    strcpy(le, "null");	le += strlen(le);
	} else {
	    size_t slen = 4*lineLength - (le - l) - sizeof("\"...\")");
	    *le++ = '"';
	    strncpy(le, s, slen); le[slen] = '\0'; le += strlen(le);	
	    if (slen < strlen(s)) {
		strcpy(le, "...");	le += strlen(le);
	    }
	    *le++ = '"';
	}
    }	break;
    case POPT_ARG_NONE:
    default:
	l = _free(l);
	return NULL;
	/*@notreached@*/ break;
    }
    *le++ = ')';
    *le = '\0';
/*@=boundswrite@*/

    return l;
}

/**
 * Display help text for an option.
 * @param fp		output file handle
 * @param columns	output display width control
 * @param opt		option(s)
 * @param translation_domain	translation domain
 */
static void singleOptionHelp(FILE * fp, columns_t columns,
		const struct poptOption * opt,
		/*@null@*/ const char * translation_domain)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    size_t maxLeftCol = columns->cur;
    size_t indentLength = maxLeftCol + 5;
    size_t lineLength = columns->max - indentLength;
    const char * help = D_(translation_domain, opt->descrip);
    const char * argDescrip = getArgDescrip(opt, translation_domain);
    size_t helpLength;
    char * defs = NULL;
    char * left;
    size_t nb = maxLeftCol + 1;
    int displaypad = 0;

    /* Make sure there's more than enough room in target buffer. */
    if (opt->longName)	nb += strlen(opt->longName);
    if (argDescrip)	nb += strlen(argDescrip);

/*@-boundswrite@*/
    left = malloc(nb);
    if (left == NULL) return;	/* XXX can't happen */
    left[0] = '\0';
    left[maxLeftCol] = '\0';

    if (opt->longName && opt->shortName)
	sprintf(left, "-%c, %s%s", opt->shortName,
		((opt->argInfo & POPT_ARGFLAG_ONEDASH) ? "-" : "--"),
		opt->longName);
    else if (opt->shortName != '\0') 
	sprintf(left, "-%c", opt->shortName);
    else if (opt->longName)
	sprintf(left, "%s%s",
		((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_MAINCALL ? "" :
		((opt->argInfo & POPT_ARGFLAG_ONEDASH) ? "-" : "--")),
		opt->longName);
    if (!*left) goto out;

    if (argDescrip) {
	char * le = left + strlen(left);

	if (opt->argInfo & POPT_ARGFLAG_OPTIONAL)
	    *le++ = '[';

	/* Choose type of output */
/*@-branchstate@*/
	if (opt->argInfo & POPT_ARGFLAG_SHOW_DEFAULT) {
	    defs = singleOptionDefaultValue(lineLength, opt, translation_domain);
	    if (defs) {
		char * t = malloc((help ? strlen(help) : 0) +
				strlen(defs) + sizeof(" "));
		if (t) {
		    char * te = t;
		    *te = '\0';
		    if (help) {
			strcpy(te, help);	te += strlen(te);
		    }
		    *te++ = ' ';
		    strcpy(te, defs);
		    defs = _free(defs);
		}
		defs = t;
	    }
	}
/*@=branchstate@*/

	if (opt->argDescrip == NULL) {
	    switch (opt->argInfo & POPT_ARG_MASK) {
	    case POPT_ARG_NONE:
		break;
	    case POPT_ARG_VAL:
#ifdef	NOTNOW	/* XXX pug ugly nerdy output */
	    {	long aLong = opt->val;
		int ops = (opt->argInfo & POPT_ARGFLAG_LOGICALOPS);
		int negate = (opt->argInfo & POPT_ARGFLAG_NOT);

		/* Don't bother displaying typical values */
		if (!ops && (aLong == 0L || aLong == 1L || aLong == -1L))
		    break;
		*le++ = '[';
		switch (ops) {
		case POPT_ARGFLAG_OR:
		    *le++ = '|';
		    /*@innerbreak@*/ break;
		case POPT_ARGFLAG_AND:
		    *le++ = '&';
		    /*@innerbreak@*/ break;
		case POPT_ARGFLAG_XOR:
		    *le++ = '^';
		    /*@innerbreak@*/ break;
		default:
		    /*@innerbreak@*/ break;
		}
		*le++ = (opt->longName != NULL ? '=' : ' ');
		if (negate) *le++ = '~';
		/*@-formatconst@*/
		le += sprintf(le, (ops ? "0x%lx" : "%ld"), aLong);
		/*@=formatconst@*/
		*le++ = ']';
	    }
#endif
		break;
	    case POPT_ARG_INT:
	    case POPT_ARG_LONG:
	    case POPT_ARG_FLOAT:
	    case POPT_ARG_DOUBLE:
	    case POPT_ARG_STRING:
		*le++ = (opt->longName != NULL ? '=' : ' ');
		strcpy(le, argDescrip);		le += strlen(le);
		break;
	    default:
		break;
	    }
	} else {
	    size_t lelen;

	    *le++ = ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_MAINCALL)
		? ' ' : '=';
	    strcpy(le, argDescrip);
	    lelen = strlen(le);
	    le += lelen;

#ifdef	POPT_WCHAR_HACK
	    {	const char * scopy = argDescrip;
		mbstate_t t;
		size_t n;

		memset ((void *)&t, 0, sizeof (t));	/* In initial state.  */
		/* Determine number of characters.  */
		n = mbsrtowcs (NULL, &scopy, strlen(scopy), &t);

		displaypad = (int) (lelen-n);
	    }
#endif
	}
	if (opt->argInfo & POPT_ARGFLAG_OPTIONAL)
	    *le++ = ']';
	*le = '\0';
    }
/*@=boundswrite@*/

    if (help)
	POPT_fprintf(fp,"  %-*s   ", (int)(maxLeftCol+displaypad), left);
    else {
	POPT_fprintf(fp,"  %s\n", left); 
	goto out;
    }

    left = _free(left);
/*@-branchstate@*/
    if (defs) {
	help = defs;
    }

    helpLength = strlen(help);
/*@-boundsread@*/
    while (helpLength > lineLength) {
	const char * ch;
	char format[16];

	ch = help + lineLength - 1;
	while (ch > help && !_isspaceptr(ch))
	    ch = POPT_prev_char (ch);
	if (ch == help) break;		/* give up */
	while (ch > (help + 1) && _isspaceptr(ch))
	    ch = POPT_prev_char (ch);
	ch++;

	sprintf(format, "%%.%ds\n%%%ds", (int) (ch - help), (int) indentLength);
	/*@-formatconst@*/
	POPT_fprintf(fp, format, help, " ");
	/*@=formatconst@*/
	help = ch;
	while (_isspaceptr(help) && *help) help++;
	helpLength = strlen(help);
    }
/*@=boundsread@*/
/*@=branchstate@*/

    if (helpLength) POPT_fprintf(fp, "%s\n", help);
    help = NULL;

out:
    /*@-dependenttrans@*/
    defs = _free(defs);
    /*@=dependenttrans@*/
    left = _free(left);
}

/**
 * Find display width for longest argument string.
 * @param opt		option(s)
 * @param translation_domain	translation domain
 * @return		display width
 */
static size_t maxArgWidth(const struct poptOption * opt,
		       /*@null@*/ const char * translation_domain)
	/*@*/
{
    size_t max = 0;
    size_t len = 0;
    const char * s;
    
    if (opt != NULL)
    while (opt->longName || opt->shortName || opt->arg) {
	if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE) {
	    if (opt->arg)	/* XXX program error */
	        len = maxArgWidth(opt->arg, translation_domain);
	    if (len > max) max = len;
	} else if (!(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN)) {
	    len = sizeof("  ")-1;
	    if (opt->shortName != '\0') len += sizeof("-X")-1;
	    if (opt->shortName != '\0' && opt->longName) len += sizeof(", ")-1;
	    if (opt->longName) {
		len += ((opt->argInfo & POPT_ARGFLAG_ONEDASH)
			? sizeof("-")-1 : sizeof("--")-1);
		len += strlen(opt->longName);
	    }

	    s = getArgDescrip(opt, translation_domain);

#ifdef POPT_WCHAR_HACK
	    /* XXX Calculate no. of display characters. */
	    if (s) {
		const char * scopy = s;
		mbstate_t t;
		size_t n;

/*@-boundswrite@*/
		memset ((void *)&t, 0, sizeof (t));	/* In initial state.  */
/*@=boundswrite@*/
		/* Determine number of characters.  */
		n = mbsrtowcs (NULL, &scopy, strlen(scopy), &t);
		len += sizeof("=")-1 + n;
	    }
#else
	    if (s)
		len += sizeof("=")-1 + strlen(s);
#endif

	    if (opt->argInfo & POPT_ARGFLAG_OPTIONAL) len += sizeof("[]")-1;
	    if (len > max) max = len;
	}

	opt++;
    }
    
    return max;
}

/**
 * Display popt alias and exec help.
 * @param fp		output file handle
 * @param items		alias/exec array
 * @param nitems	no. of alias/exec entries
 * @param left		largest argument display width
 * @param translation_domain	translation domain
 */
static void itemHelp(FILE * fp,
		/*@null@*/ poptItem items, int nitems,
		columns_t columns,
		/*@null@*/ const char * translation_domain)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    poptItem item;
    int i;

    if (items != NULL)
    for (i = 0, item = items; i < nitems; i++, item++) {
	const struct poptOption * opt;
	opt = &item->option;
	if ((opt->longName || opt->shortName) && 
	    !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN))
	    singleOptionHelp(fp, columns, opt, translation_domain);
    }
}

/**
 * Display help text for a table of options.
 * @param con		context
 * @param fp		output file handle
 * @param table		option(s)
 * @param columns	output display width control
 * @param translation_domain	translation domain
 */
static void singleTableHelp(poptContext con, FILE * fp,
		/*@null@*/ const struct poptOption * table,
		columns_t columns,
		/*@null@*/ const char * translation_domain)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    const struct poptOption * opt;
    const char *sub_transdom;

    if (table == poptAliasOptions) {
	itemHelp(fp, con->aliases, con->numAliases, columns, NULL);
	itemHelp(fp, con->execs, con->numExecs, columns, NULL);
	return;
    }

    if (table != NULL)
    for (opt = table; (opt->longName || opt->shortName || opt->arg); opt++) {
	if ((opt->longName || opt->shortName) && 
	    !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN))
	    singleOptionHelp(fp, columns, opt, translation_domain);
    }

    if (table != NULL)
    for (opt = table; (opt->longName || opt->shortName || opt->arg); opt++) {
	if ((opt->argInfo & POPT_ARG_MASK) != POPT_ARG_INCLUDE_TABLE)
	    continue;
	sub_transdom = getTableTranslationDomain(opt->arg);
	if (sub_transdom == NULL)
	    sub_transdom = translation_domain;
	    
	if (opt->descrip)
	    POPT_fprintf(fp, "\n%s\n", D_(sub_transdom, opt->descrip));

	singleTableHelp(con, fp, opt->arg, columns, sub_transdom);
    }
}

/**
 * @param con		context
 * @param fp		output file handle
 */
static size_t showHelpIntro(poptContext con, FILE * fp)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    size_t len = (size_t)6;
    const char * fn;

    fprintf(fp, POPT_("Usage:"));
    if (!(con->flags & POPT_CONTEXT_KEEP_FIRST)) {
/*@-boundsread@*/
/*@-type@*/	/* LCL: wazzup? */
	fn = con->optionStack->argv[0];
/*@=type@*/
/*@=boundsread@*/
	if (fn == NULL) return len;
	if (strchr(fn, '/')) fn = strrchr(fn, '/') + 1;
	fprintf(fp, " %s", fn);
	len += strlen(fn) + 1;
    }

    return len;
}

void poptPrintHelp(poptContext con, FILE * fp, /*@unused@*/ int flags)
{
    columns_t columns = memset(alloca(sizeof(*columns)), 0, sizeof(*columns));

    (void) showHelpIntro(con, fp);
    if (con->otherHelp)
	fprintf(fp, " %s\n", con->otherHelp);
    else
	fprintf(fp, " %s\n", POPT_("[OPTION...]"));

    columns->cur = maxArgWidth(con->options, NULL);
    columns->max = maxColumnWidth(fp);
    singleTableHelp(con, fp, con->options, columns, NULL);
}

/**
 * Display usage text for an option.
 * @param fp		output file handle
 * @param columns	output display width control
 * @param opt		option(s)
 * @param translation_domain	translation domain
 */
static size_t singleOptionUsage(FILE * fp, columns_t columns,
		const struct poptOption * opt,
		/*@null@*/ const char *translation_domain)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    size_t len = (size_t)4;
    char shortStr[2] = { '\0', '\0' };
    const char * item = shortStr;
    const char * argDescrip = getArgDescrip(opt, translation_domain);
    int bingo = 0;

    if (opt->shortName != '\0' && opt->longName != NULL) {
	len += 2;
	if (!(opt->argInfo & POPT_ARGFLAG_ONEDASH)) len++;
	len += strlen(opt->longName);
	bingo++;
    } else if (opt->shortName != '\0') {
	len++;
	shortStr[0] = opt->shortName;
	shortStr[1] = '\0';
	bingo++;
    } else if (opt->longName) {
	len += strlen(opt->longName);
	if (!(opt->argInfo & POPT_ARGFLAG_ONEDASH)) len++;
	item = opt->longName;
	bingo++;
    }

    if (!bingo) return columns->cur;

#ifdef POPT_WCHAR_HACK
    /* XXX Calculate no. of display characters. */
    if (argDescrip) {
	const char * scopy = argDescrip;
	mbstate_t t;
	size_t n;

/*@-boundswrite@*/
	memset ((void *)&t, 0, sizeof (t));	/* In initial state.  */
/*@=boundswrite@*/
	/* Determine number of characters.  */
	n = mbsrtowcs (NULL, &scopy, strlen(scopy), &t);
	len += sizeof("=")-1 + n;
    }
#else
    if (argDescrip) 
	len += sizeof("=")-1 + strlen(argDescrip);
#endif

    if ((columns->cur + len) > columns->max) {
	fprintf(fp, "\n       ");
	columns->cur = (size_t)7;
    } 

    if (opt->longName && opt->shortName) {
	fprintf(fp, " [-%c|-%s%s%s%s]",
	    opt->shortName, ((opt->argInfo & POPT_ARGFLAG_ONEDASH) ? "" : "-"),
	    opt->longName,
	    (argDescrip ? " " : ""),
	    (argDescrip ? argDescrip : ""));
    } else {
	fprintf(fp, " [-%s%s%s%s]",
	    ((opt->shortName || (opt->argInfo & POPT_ARGFLAG_ONEDASH)) ? "" : "-"),
	    item,
	    (argDescrip ? (opt->shortName != '\0' ? " " : "=") : ""),
	    (argDescrip ? argDescrip : ""));
    }

    return columns->cur + len + 1;
}

/**
 * Display popt alias and exec usage.
 * @param fp		output file handle
 * @param columns	output display width control
 * @param item		alias/exec array
 * @param nitems	no. of ara/exec entries
 * @param translation_domain	translation domain
 */
static size_t itemUsage(FILE * fp, columns_t columns,
		/*@null@*/ poptItem item, int nitems,
		/*@null@*/ const char * translation_domain)
	/*@globals fileSystem @*/
	/*@modifies *fp, fileSystem @*/
{
    int i;

/*@-branchstate@*/		/* FIX: W2DO? */
    if (item != NULL)
    for (i = 0; i < nitems; i++, item++) {
	const struct poptOption * opt;
	opt = &item->option;
        if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INTL_DOMAIN) {
	    translation_domain = (const char *)opt->arg;
	} else if ((opt->longName || opt->shortName) &&
		 !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN)) {
	    columns->cur = singleOptionUsage(fp, columns, opt, translation_domain);
	}
    }
/*@=branchstate@*/

    return columns->cur;
}

/**
 * Keep track of option tables already processed.
 */
typedef struct poptDone_s {
    int nopts;
    int maxopts;
    const void ** opts;
} * poptDone;

/**
 * Display usage text for a table of options.
 * @param con		context
 * @param fp		output file handle
 * @param columns	output display width control
 * @param opt		option(s)
 * @param translation_domain	translation domain
 * @param done		tables already processed
 * @return
 */
static size_t singleTableUsage(poptContext con, FILE * fp, columns_t columns,
		/*@null@*/ const struct poptOption * opt,
		/*@null@*/ const char * translation_domain,
		/*@null@*/ poptDone done)
	/*@globals fileSystem @*/
	/*@modifies *fp, done, fileSystem @*/
{
/*@-branchstate@*/		/* FIX: W2DO? */
    if (opt != NULL)
    for (; (opt->longName || opt->shortName || opt->arg) ; opt++) {
        if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INTL_DOMAIN) {
	    translation_domain = (const char *)opt->arg;
	} else if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE) {
	    if (done) {
		int i = 0;
		for (i = 0; i < done->nopts; i++) {
/*@-boundsread@*/
		    const void * that = done->opts[i];
/*@=boundsread@*/
		    if (that == NULL || that != opt->arg)
			/*@innercontinue@*/ continue;
		    /*@innerbreak@*/ break;
		}
		/* Skip if this table has already been processed. */
		if (opt->arg == NULL || i < done->nopts)
		    continue;
/*@-boundswrite@*/
		if (done->nopts < done->maxopts)
		    done->opts[done->nopts++] = (const void *) opt->arg;
/*@=boundswrite@*/
	    }
	    columns->cur = singleTableUsage(con, fp, columns, opt->arg,
			translation_domain, done);
	} else if ((opt->longName || opt->shortName) &&
		 !(opt->argInfo & POPT_ARGFLAG_DOC_HIDDEN)) {
	    columns->cur = singleOptionUsage(fp, columns, opt, translation_domain);
	}
    }
/*@=branchstate@*/

    return columns->cur;
}

/**
 * Return concatenated short options for display.
 * @todo Sub-tables should be recursed.
 * @param opt		option(s)
 * @param fp		output file handle
 * @retval str		concatenation of short options
 * @return		length of display string
 */
static size_t showShortOptions(const struct poptOption * opt, FILE * fp,
		/*@null@*/ char * str)
	/*@globals fileSystem @*/
	/*@modifies *str, *fp, fileSystem @*/
	/*@requires maxRead(str) >= 0 @*/
{
    /* bufsize larger then the ascii set, lazy allocation on top level call. */
    size_t nb = (size_t)300;
    char * s = (str != NULL ? str : calloc(1, nb));
    size_t len = (size_t)0;

    if (s == NULL)
	return 0;

/*@-boundswrite@*/
    if (opt != NULL)
    for (; (opt->longName || opt->shortName || opt->arg); opt++) {
	if (opt->shortName && !(opt->argInfo & POPT_ARG_MASK))
	    s[strlen(s)] = opt->shortName;
	else if ((opt->argInfo & POPT_ARG_MASK) == POPT_ARG_INCLUDE_TABLE)
	    if (opt->arg)	/* XXX program error */
		len = showShortOptions(opt->arg, fp, s);
    } 
/*@=boundswrite@*/

    /* On return to top level, print the short options, return print length. */
    if (s != str && *s != '\0') {
	fprintf(fp, " [-%s]", s);
	len = strlen(s) + sizeof(" [-]")-1;
    }
    if (s != str)
	free(s);
    return len;
}

void poptPrintUsage(poptContext con, FILE * fp, /*@unused@*/ int flags)
{
    columns_t columns = memset(alloca(sizeof(*columns)), 0, sizeof(*columns));
    struct poptDone_s done_buf;
    poptDone done = &done_buf;

    memset(done, 0, sizeof(*done));
    done->nopts = 0;
    done->maxopts = 64;
    columns->cur = done->maxopts * sizeof(*done->opts);
    columns->max = maxColumnWidth(fp);
/*@-boundswrite@*/
    done->opts = calloc(1, columns->cur);
    /*@-keeptrans@*/
    done->opts[done->nopts++] = (const void *) con->options;
    /*@=keeptrans@*/
/*@=boundswrite@*/

    columns->cur = showHelpIntro(con, fp);
    columns->cur += showShortOptions(con->options, fp, NULL);
    columns->cur = singleTableUsage(con, fp, columns, con->options, NULL, done);
    columns->cur = itemUsage(fp, columns, con->aliases, con->numAliases, NULL);
    columns->cur = itemUsage(fp, columns, con->execs, con->numExecs, NULL);

    if (con->otherHelp) {
	columns->cur += strlen(con->otherHelp) + 1;
	if (columns->cur > columns->max) fprintf(fp, "\n       ");
	fprintf(fp, " %s", con->otherHelp);
    }

    fprintf(fp, "\n");
    free(done->opts);
}

void poptSetOtherOptionHelp(poptContext con, const char * text)
{
    con->otherHelp = _free(con->otherHelp);
    con->otherHelp = xstrdup(text);
}
