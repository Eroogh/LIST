#ifndef LIST_H 
#define LIST_H
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>

typedef struct
{
    double *data;
    long long unsigned *next;
    long long unsigned *prev;
    long long unsigned free;
    size_t size;
} List;

typedef struct 
{
    double val;
    long long unsigned next;
    long long unsigned prev;
} Elem;

typedef struct
{
    Elem *elems;
    long long unsigned free;
    size_t size;

} StructList;

typedef enum 
{
    NOERROR = 0,
    POISONERR,
    CANARYERR,
    HEADERR,
    FREEVALERR,
    FREEPREVERR
}
LISTERRNUM;

const int PoisonVal = 666666;
const int CanaryVal = 0xC1A551C;
const char* LogWidth = "2000";

#define STRUCTLISTINIT(list, size, htmfile)                  StructListInit(list, size, htmfile, __LINE__)
#define STRUCTLISTVERIFY(list)                               StructListVerify(list, __LINE__, stderr)
#define STRUCTLISTDUMP(list, filename)                       StructListDump(list, filename, __LINE__)
#define STRUCTLISTINSERTAFTER(list, index, val, htmfile)     StructListInsertAfter(list, index, val, htmfile, __LINE__)
#define STRUCTLISTINSERTTOTHESTART(list, val, htmfile)       StructListInsertToTheStart(list, val, htmfile, __LINE__)
#define STRUCTLISTINSERTTOTHEEND(list, val, htmfile)         StructListInsertToTheEnd(list, val, htmfile, __LINE__)
#define STRUCTLISTINSERTBEFORE(list, index, val, htmfile)    StructListInsertBefore(list, index, val, htmfile, __LINE__)
#define STRUCTLISTREMOVE(list, index, htmfile)               StructListRemove(list, index, htmfile, __LINE__)
#define STRUCTLISTDESTROY(list, htmfile)                     StructListDestroy(list, htmfile, __LINE__)

LISTERRNUM StructListInit(StructList *list, size_t size, FILE *htmfile = 0, long unsigned line = 0);
LISTERRNUM StructListVerify(StructList *list, const char * funcname, long unsigned line = 0, FILE * WhereToPrint = stderr);
LISTERRNUM SilentListVerify(StructList *list);
void StructListDestroy(StructList *list, FILE *htmfile = 0, long unsigned line = 0);

void StructListDump(StructList *list, const char *filename, long unsigned line = 0);
void DrawHeader(FILE *output);
void DrawNode(FILE *output, StructList *list, size_t i, int scale);
void DrawEdge(FILE *output, StructList *list, size_t i, int scale);
void DrawEdgeForFreeElems(FILE *output, StructList *list, size_t i, int scale);
void DrawEdge0(FILE *output, StructList *list, int scale);

LISTERRNUM StructListLogWrite(StructList *list, FILE *htmfile, long unsigned line = 0, bool ListDestroyed = false);
void LogStart(FILE *htmfile);
LISTERRNUM LogDump(FILE *htmfile, StructList *list, char *DumpFileName, long unsigned line = 0);

// LISTERRNUM ListInsertAfter(List *list, long long index, long long val, long unsigned line = 0);

LISTERRNUM StructListInsertAfter(StructList *list, long long unsigned index, long long val, FILE *htmfile = 0, long unsigned line = 0);
LISTERRNUM StructListInsertToTheStart(StructList *list, long long val, FILE *htmfile = 0, long unsigned line = 0);
LISTERRNUM StructListInsertToTheEnd(StructList *list, long long val, FILE *htmfile = 0, long unsigned line = 0);
LISTERRNUM StructListInsertBefore(StructList *list, long long unsigned index, long long val, FILE *htmfile = 0, long unsigned line = 0);
LISTERRNUM StructListRemove(StructList *list, long long unsigned index, FILE *htmfile = 0, long unsigned line = 0);

long long UnFreeTheElemAfter(StructList *list, long long unsigned index);
long long UnFreeTheElemBefore(StructList *list, long long unsigned index);
long long FreeTheElem(StructList *list, long long unsigned index);




#endif