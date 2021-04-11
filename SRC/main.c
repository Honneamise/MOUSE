#include "mouse.h"

/**********/
int main (int argc, char **argv)
{
    if(argc!=2)
    {
        printf("USAGE : EXE filename\n");
        exit(EXIT_FAILURE);
    }

    mouse m = {0};

    LOAD(&m, argv[1]);

    INIT(&m);

    RUN(&m);

    return 0;
}