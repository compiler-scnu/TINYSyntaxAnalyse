#ifndef GETSYNTAXTREE_H
#define GETSYNTAXTREE_H

/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "parse.h"
#include "scan.h"
#include "util.h"

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;

int Error = FALSE;

void getSyntaxTree()
{
    TreeNode * syntaxTree;

    /* source code file name */
    char pgm[]="SAMPLE.txt";
    source = fopen(pgm,"rt");

    syntaxTree = parse();

    char ch[]="syntaxTree.txt";  //语法树输出文件
    listing  = fopen(ch,"wt+");
    if (TraceParse)
    {
        fprintf(listing, "Syntax tree:\n");
        printTree(syntaxTree);
        fclose(listing);
    }

    fclose(source);
}



#endif // GETSYNTAXTREE_H
