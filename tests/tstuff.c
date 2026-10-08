#include <stdio.h>
#include <popt.h>

int main(int argc, char *argv[])
{
    poptContext ctx = poptGetContext(argv[0], argc, (const char **)argv, NULL, 0);
    int rc = 0;
    for (int i = 0; i < 100; ++i) {
	const char *ea[] = { "a", NULL };
	if ((rc = poptStuffArgs(ctx, ea)))
	    break;
    }
    printf("%d\n", rc);

    poptFreeContext(ctx);
    return rc;
}
