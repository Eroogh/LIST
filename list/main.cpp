#include "list.h"

int main()
{
    StructList list;

    FILE *htmfile = fopen("LogStuff/ListLogs.htm", "w");

    STRUCTLISTINIT(&list, 10, htmfile);

    STRUCTLISTINSERTTOTHEEND(&list, 10, htmfile);

    STRUCTLISTINSERTTOTHEEND(&list, 30, htmfile);

    STRUCTLISTINSERTBEFORE(&list, 2, 666666, htmfile);

    STRUCTLISTDESTROY(&list, htmfile);

    fclose(htmfile);

    system("xdg-open LogStuff/ListLogs.htm");
    return 0;
}