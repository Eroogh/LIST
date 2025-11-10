#include "list.h"

long long My_abs(long long a)
{
    if (a >= 0)
    {
        return a;
    }
    return -1*a;
}

LISTERRNUM StructListInit(StructList *list, size_t size, FILE *htmfile, long unsigned line)
{
    assert(list);
    list->elems = (Elem *)calloc(size, sizeof(Elem));
    list->elems[0].next = 0;
    list->elems[0].prev = 0;
    list->elems[0].val = CanaryVal;
    for (size_t i = 1; i < size; i++)
    {
        list->elems[i].val = PoisonVal;
        list->elems[i].next = i + 1;
        list->elems[i].prev = PoisonVal;
    }
    list->free = 1;
    list->elems[list->free].prev = PoisonVal;
    list->size = size;

    return StructListLogWrite(list, htmfile, line);
}

void StructListDestroy(StructList* list, FILE *htmfile, long unsigned line)
{
    StructListLogWrite(list, htmfile, line, true);
    free(list->elems);
}

//------------------------------------

LISTERRNUM StructListInsertAfter(StructList *list, long long unsigned index, long long val, FILE *htmfile, long unsigned line)
{
    assert(index <= list->size);
    StructListVerify(list, __func__, line);

    list->elems[list->free].val = val;
    list->elems[list->elems[index].next].prev = list->free; 
    list->elems[index].next = UnFreeTheElemAfter(list, index);

    return StructListLogWrite(list, htmfile, line); 
}

long long UnFreeTheElemAfter(StructList *list, long long unsigned index)
{
    long long oldfree = list -> free;
    long long newfree = list->elems[list->free].next;

    list->elems[list->elems[list->free].next].prev = PoisonVal;
    list->elems[list->free].prev = index;
    list->elems[list->free].next = list->elems[index].next;
    list->free = newfree;

    return oldfree;
}

LISTERRNUM StructListInsertBefore(StructList *list, long long unsigned index, long long val, FILE *htmfile, long unsigned line)
{
    assert(index <= list->size);
    StructListVerify(list, __func__, line);

    list->elems[list->free].val = val;
    list->elems[list->elems[index].prev].next = list->free; 
    list->elems[index].prev = UnFreeTheElemBefore(list, index);

    return StructListLogWrite(list, htmfile, line); 
}

long long UnFreeTheElemBefore(StructList *list, long long unsigned index)
{
    long long oldfree = list->free;
    long long newfree = list->elems[list->free].next;

    list->elems[list->free].prev = list->elems[index].prev;
    list->elems[list->elems[list->free].next].prev = PoisonVal;
    list->elems[list->free].next = index;
    list->free = newfree;

    return oldfree;
}

LISTERRNUM StructListInsertToTheStart(StructList *list, long long val, FILE *htmfile, long unsigned line)
{
    StructListVerify(list, __func__, line);
    return StructListInsertBefore(list, list->elems[0].next, val, htmfile, line);
}

LISTERRNUM StructListInsertToTheEnd(StructList *list, long long val, FILE *htmfile, long unsigned line)
{
    StructListVerify(list, __func__, line);
    return StructListInsertAfter(list, list->elems[0].prev, val, htmfile, line);
}

LISTERRNUM StructListRemove(StructList *list, long long unsigned index, FILE *htmfile, long unsigned line)
{
    assert(index <= list->size);
    StructListVerify(list, __func__, line);

    list->elems[list->elems[index].next].prev = list->elems[index].prev;
    list->elems[list->elems[index].prev].next = list->elems[index].next;

    FreeTheElem(list, index);

    return StructListLogWrite(list, htmfile, line); 

}

long long FreeTheElem(StructList *list, long long unsigned index)
{
    list->elems[index].prev = PoisonVal;
    list->elems[index].next = list->free;
    list->elems[index].val  = PoisonVal;
    list->free = index;

    return 0;
}

//------------------------------------

LISTERRNUM StructListVerify(StructList *list, const char * funcname, long unsigned line, FILE * WhereToPrint)
{
    assert(list);
    assert(list->size);
    assert(list->free);

    if (list->elems[0].val != CanaryVal)
    {
        fprintf(WhereToPrint, "Canary died !!!, line = %lu, in %s\n\n", line, funcname);
        return CANARYERR;
    }

    size_t counter = 0;

    for (size_t i = list->elems[0].next; i != 0; i = list->elems[i].next)
    {
        if (list->elems[i].val == PoisonVal)
        {
            fprintf(WhereToPrint, "There's posion in list !!!, line = %lu, in %s\n\n", line, funcname);
            return POISONERR;
        }
        if (counter > list->size)
        {
            fprintf(WhereToPrint, "Cant find tail going from head !!!, line = %lu, in %s\n\n", line, funcname);
            return HEADERR;
        }
        counter++;
    }
    for (size_t i = list->free; i < list->size; i = list->elems[i].next)
    {
        if (list->elems[i].val != PoisonVal)
        {
            fprintf(WhereToPrint, "Free list element is changed !!!, line = %lu, in %s\n\n", line, funcname);
            return FREEVALERR;
        }
        if (list->elems[i].prev != PoisonVal)
        {
            fprintf(WhereToPrint, "Free list element's prev is changed !!!, line = %lu, %s\n\n", line, funcname);
            return FREEPREVERR;
        }
    }
    return NOERROR;
}

LISTERRNUM SilentListVerify(StructList *list)
{
    assert(list);
    assert(list->size);
    assert(list->free);

    if (list->elems[0].val != CanaryVal)
    {
        return CANARYERR;
    }

    size_t counter = 0;

    for (size_t i = list->elems[0].next; i != 0; i = list->elems[i].next)
    {
        if (list->elems[i].val == PoisonVal)
        {
            return POISONERR;
        }
        if (counter > list->size)
        {
            return HEADERR;
        }
        counter++;
    }
    for (size_t i = list->free; i < list->size; i = list->elems[i].next)
    {
        if (list->elems[i].val != PoisonVal)
        {
            return FREEVALERR;
        }
        if (list->elems[i].prev != PoisonVal)
        {
            return FREEPREVERR;
        }
    }
    return NOERROR;
}

//------------------------------------ TODO sleep more, print function's name in logs

#define LogWidth "2000"

LISTERRNUM StructListLogWrite(StructList *list, FILE *htmfile, long unsigned line, bool ListDestroyed)
{
    assert(list);
    if (htmfile == 0)
    {
        return StructListVerify(list, __func__, line);
    }

    if (ListDestroyed)
    {
        fprintf(htmfile,"</body>\n"
                        "</html>\n"
                );
        return NOERROR;
    }

    static int NumOfDumps = 0;

    if (NumOfDumps == 0)
    {
        LogStart(htmfile);
        NumOfDumps++;
        return NOERROR;
    }
    
    char DumpFileName[100] = {};
    sprintf(DumpFileName, "Dump_%d", NumOfDumps);
    LISTERRNUM err = LogDump(htmfile, list, DumpFileName, line);

    NumOfDumps++;
    return err;
}

void LogStart(FILE *htmfile)
{
    fprintf(htmfile,"<!DOCTYPE html>\n"
                    "<html lang=\"en\">\n"
                    "<head>\n"
                    "\t<title>My Log</title>\n"
                    "</head>\n\n"
                    "<body>\n"
            );
}

LISTERRNUM LogDump(FILE *htmfile, StructList *list,  char* DumpFileName, long unsigned line)
{
    assert(htmfile);
    assert(DumpFileName);

    fprintf(htmfile,"\t<h2>DUMP</h2>\n"
                    "\t<pre>\n"     
            );

    if (SilentListVerify(list) != NOERROR)
    {
        fprintf(htmfile, "\t<p style=\"color:Red;\">");
    }
    else
    {
        fprintf(htmfile, "\t<p>");
    }

    LISTERRNUM err =  StructListVerify(list, __func__, line, htmfile);

    fprintf(htmfile,"\nline = %lu\n"
                    "here's your list:</p>\n"
                    "\t</pre>\n"
                    "\t<img src=\"%s.svg\" width = \"" LogWidth "\"></img>\n",
                    line, DumpFileName
            );

    char LogStuffDumpFileName[100] = {};

    sprintf(LogStuffDumpFileName, "LogStuff/%s", DumpFileName);

    printf("%s\n", LogStuffDumpFileName);

    StructListDump(list, LogStuffDumpFileName, line);

    return err;
}

//------------------------------------

#define color1 "#ff3030ff" 
#define color2 "#1001b8ff"
#define color3 "#0d8600ff"

void StructListDump(StructList *list, const char* filename, long unsigned line)
{
    assert(filename);

    char filenamewithdot[100] = {};

    sprintf(filenamewithdot, "%s.dot", filename);

    //fprintf(stderr, "%s %s\n", filename, filenamewithdot);

    FILE *output = fopen(filenamewithdot, "w");

    int scale = 3;

    DrawHeader(output);

    for (size_t i = 0; i < list->size; i++)
    {
        DrawNode(output, list, i, scale);
    }
    for (size_t i = 0; i < list->size - 1; i++)
    {
        if (i == list->free)
        {
        fprintf(output,
                "\tfree [pos=\"%zu,0.6!\", label=\"free\"];\n\n",
                scale*(i));
        }
        if (list->elems[i].prev == PoisonVal)
        {
            DrawEdgeForFreeElems(output, list, i, scale);
            continue;
        }
        fprintf(output,
                "\telem%zu -> elem%zu [style=invis, weight=100];\n\n", 
                i, i + 1);
                
        DrawEdge(output, list, i, scale);
    }
    DrawEdge0(output, list, scale);

    fprintf(output, "}");
    fclose(output);

    char makesvg[300] = {};

    sprintf(makesvg, "dot -Tsvg %s.dot > %s.svg", filename, filename);

    system(makesvg);
}

void DrawHeader(FILE * output)
{
    fprintf(output,
            "digraph List {\n"
            "\trankdir = LR;\n\n"
            "\tlayout = neato;\n\n"
            "\tsplines = curved;\n\n"
            "\tnode [shape=none, fontname=\"Impact\", fontsize=14];\n\n");
    
}

void DrawNode(FILE * output, StructList * list, size_t i, int scale)
{
    fprintf(output, 
            "\telem%zu [pos=\"%zu,0!\", label = <<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n"
                "\t\t<TR><TD BGCOLOR=\"white\" COLSPAN=\"2\">index = %zu</TD></TR>\n"
                "\t\t<TR><TD BGCOLOR=\"#4985f5ff\" COLSPAN=\"2\">%lg</TD></TR>\n"
                "\t\t<TR><TD BGCOLOR=\"red\" PORT=\"prev%zu\">prev = %llu</TD>\n"
                    "\t\t\t<TD BGCOLOR=\"red\" PORT=\"next%zu\">next = %llu</TD></TR>\n"
            "\t\t</TABLE>>];\n\n",
            i, scale*i, i, list->elems[i].val, i, list->elems[i].prev, i, list->elems[i].next);

}

void DrawEdge(FILE * output, StructList * list, size_t i, int scale)
{
    if (My_abs(i - list->elems[i].next) > 1)
        {
        fprintf(output,
                "\tfictnext%zu [shape = rectangle, pos=\"%llu,%ld!\", label=\"\", width=0, height=0, color=\"" color1 "\"];\n\n",
                i, scale*(i + list->elems[i].next)/2, -(long)(My_abs(list->elems[i].next - i)));

        fprintf(output,
                "\telem%zu:<next%zu>:s -> fictnext%zu [arrowhead = none, color=\"" color1 "\", constraint=false];\n\n",
                i, i, i);

        fprintf(output,
                "\tfictnext%zu -> elem%llu:<prev%llu>:s [color=\"" color1 "\", constraint=false];\n\n",
                i, list->elems[i].next, list->elems[i].next);
        if(list->elems[i + 1].prev != PoisonVal)
        {
        fprintf(output,
                "\tfictprev%zu [shape = rectangle, pos=\"%llu,%ld!\", label=\"\", width=0, height=0, color=\"" color2 "\"];\n\n",
                i, scale*(i + 1 + list->elems[i + 1].prev)/2, -(long)(My_abs(list->elems[i + 1].prev - i - 1)+ 1));

        fprintf(output,
                "\telem%zu:<prev%zu>:s -> fictprev%zu [arrowhead = none, color=\"" color2 "\", constraint=false];\n\n",
                i + 1, i + 1, i);

        fprintf(output,
                "\tfictprev%zu -> elem%llu:<next%llu>:s [color=\"" color2 "\", constraint=false];\n\n",
                i, list->elems[i + 1].prev, list->elems[i + 1].prev);
        }
        }
        else
        {
        if ( i > list->elems[i].next)
        {
        fprintf(output,
                    "\tfictnext%zu [shape = rectangle, pos=\"%llu,%ld!\", label=\"\", width=0, height=0, color=\"" color1 "\"];\n\n",
                    i, scale*(i + list->elems[i].next)/2, -(long)(My_abs(list->elems[i].next - i)));

        fprintf(output,
                "\telem%zu:<next%zu>:s -> fictnext%zu [arrowhead = none, color=\"" color1 "\", constraint=false];\n\n",
                i, i, i);

        fprintf(output,
                "\tfictnext%zu -> elem%llu:<prev%llu>:s [color=\"" color1 "\", constraint=false];\n\n",
                i, list->elems[i].next, list->elems[i].next);
        } 
        else
        {                    
        fprintf(output,
                "\telem%zu:<next%zu> -> elem%llu:<prev%llu> [color=\"" color1 "\", constraint=false];\n\n", 
                i, i, list->elems[i].next, list->elems[i].next);
        }
        if(list->elems[i + 1].prev != PoisonVal)
        {
        fprintf(output,
                "\telem%zu:<prev%zu> -> elem%llu:<next%llu> [color=\"" color2 "\", constraint=false];\n\n", 
                i + 1, i + 1, list->elems[i + 1].prev, list->elems[i + 1].prev);
        }
        }
    
}

void DrawEdgeForFreeElems(FILE * output, StructList * list, size_t i, int scale)
{
    if (My_abs(i - list->elems[i].next) > 1)
    {
    fprintf(output,
                "\tfictnext%zu [shape = rectangle, pos=\"%llu,%ld!\", label=\"\", width=0, height=0, color=\"" color3 "\"];\n\n",
                i, scale*(i + list->elems[i].next)/2, -(long)(My_abs(list->elems[i].next - i)));

    fprintf(output,
            "\telem%zu:<next%zu>:s -> fictnext%zu [arrowhead = none, color=\"" color3 "\", constraint=false];\n\n",
            i, i, i);

    fprintf(output,
            "\tfictnext%zu -> elem%llu:<prev%llu>:s [color=\"" color3 "\", constraint=false];\n\n",
            i, list->elems[i].next, list->elems[i].next);
    } 
    else
    {
    fprintf(output,
            "\telem%zu:<next%zu> -> elem%llu:<prev%llu> [color=\"" color3 "\", constraint=false];\n\n",
            i, i, list->elems[i].next, list->elems[i].next);
    }

}

void DrawEdge0(FILE * output, StructList * list, int scale)
{
    fprintf(output,
                    "\tfictthing [shape = rectangle, pos=\"%llu,%ld!\", label=\"\", width=0, height=0, color=\"" color2  "\"];\n\n",
                    scale*(list->elems[0].prev)/2, -(long)(My_abs(list->elems[0].prev)+ 1));

    fprintf(output,
            "\thead [pos=\"%llu,0.6!\", label=\"head\"];\n\n", 
            scale*list->elems[0].next);

    fprintf(output,
            "\ttail [pos=\"%llu,0.6!\", label=\"tail\"];\n\n", 
            scale*list->elems[0].prev);

    fprintf(output,
            "\telem0:<prev0> -> fictthing [arrowhead=none, color=\"" color2 "\", constraint=false];\n\n");

    fprintf(output,
            "\tfictthing -> elem%llu:<next%llu>  [color=\"" color2 "\", constraint=false];\n\n",
            list->elems[0].prev, list->elems[0].prev);
}