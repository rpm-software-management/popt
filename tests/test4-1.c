#include <stdio.h>
#include <popt.h>

static const char *arg2 = "meh";

static struct poptOption options[] = {
    { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaargh",
	    '\0', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT,
	    &arg2, 0, "Second", NULL },

    POPT_AUTOALIAS
    POPT_AUTOHELP
    POPT_TABLEEND
};

int main(int argc, const char *argv[])
{
    int rc;
    poptContext optCon = poptGetContext("test4", argc, argv, options, 0);

    poptReadDefaultConfig(optCon, 0);

    rc = poptGetNextOpt(optCon);

    if (rc < -1) {
	fprintf(stderr, "%s: bad argument %s: %s\n",
		argv[0], poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
		poptStrerror(rc));
	goto exit;
    }

exit:
    poptFreeContext(optCon);
}
