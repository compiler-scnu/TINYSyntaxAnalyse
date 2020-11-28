/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "util.h"
#include "scan.h"
#include "parse.h"
#include <QDebug>

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * power_term(void); //增加一个优先级处理乘方
static TreeNode * factor(void);

/* 新增的树节点 */
static TreeNode * while_stmt(void);
static TreeNode * dowhile_stmt(void);
static TreeNode * for_stmt(void);

static void syntaxError(const char * message)
{
    fprintf(listing, "\n>>> ");
    fprintf(listing, "Syntax error at line %d: %s", lineno, message);
    Error = TRUE;
}

static void match(TokenType expected)
{
    if (token == expected)
        token = getToken();
    else
    {
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        fprintf(listing, "      ");
    }
}

TreeNode * stmt_sequence(void)
{
    TreeNode * t = statement();
    TreeNode * p = t;
    while ((token!=ENDFILE) && (token!=END) &&
           (token!=ELSE) && (token!=UNTIL))
    {
        TreeNode * q;
        match(SEMI);
        q = statement();
        if (q!=NULL)
        {
            if (t==NULL)
                t = p = q;
            else /* now p cannot be NULL either */
            {
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}


//P394
//lineno: 961
TreeNode * statement(void)
{
    TreeNode * t = NULL;
    switch (token)
    {
    case IF : t = if_stmt(); break;
    case REPEAT : t = repeat_stmt(); break;
    case ID : t = assign_stmt(); break;
    case READ : t = read_stmt(); break;
    case WRITE : t = write_stmt(); break;

    /* 扩充的文法 */
    case WHILE: t = while_stmt(); break;
    case DO: t = dowhile_stmt(); break;
    case FOR: t = for_stmt(); break;

    default :
        syntaxError("unexpected token -> ");
        printToken(token,tokenString);
        token = getToken();
        break;
    } /* end case */
    return t;
}


//P394
//lineno: 977
/* 改写if文法 */
TreeNode * if_stmt(void)
{
    TreeNode * t = newStmtNode(IfK);
    match(IF);

    if (t != NULL)
    {
        match(LPAREN);
        t->child[0] = exp();
        match(RPAREN);
    }


    if (t != NULL)
        t->child[1] = stmt_sequence();
    if (token == ELSE)
    {
        match(ELSE);
        if (t != NULL)
            t->child[2] = stmt_sequence();
    }
    return t;
}

//P394
//lineno:991
TreeNode * repeat_stmt(void)
{
    TreeNode * t = newStmtNode(RepeatK);
    match(REPEAT);
    if (t != NULL)
        t->child[0] = stmt_sequence();
    match(UNTIL);
    if (t != NULL)
        t->child[1] = exp();
    return t;
}

TreeNode * assign_stmt(void)
{
    TreeNode * t = newStmtNode(AssignK);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    if(token == ASSIGN)
    {
        match(ASSIGN);
        if (t != NULL)
            t->child[0] = exp();
    }
    if(token == PLUSEQUAL) //处理 +=
    {
        t->kind.stmt = PlusEqualK;
        match(PLUSEQUAL);
        if(t != NULL)
            t->child[0] = exp();
    }

    return t;
}

TreeNode * read_stmt(void)
{
    TreeNode * t = newStmtNode(ReadK);
    match(READ);
    if ((t != NULL) && (token == ID))
        t->attr.name = copyString(tokenString);
    match(ID);
    return t;
}

TreeNode * write_stmt(void)
{
    TreeNode * t = newStmtNode(WriteK);
    match(WRITE);
    if (t != NULL)
        t->child[0] = exp();
    return t;
}

TreeNode * exp(void)
{
    TreeNode * t = simple_exp();
    if ((token == LT) || (token == EQ) || (token == LESSEQUAL) || (token == NEQ)) // < = <= <>
    {
        TreeNode * p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
        }
        match(token);
        if (t != NULL)
            t->child[1] = simple_exp();
    }
    return t;
}

TreeNode * simple_exp(void)
{
    TreeNode * t = term();
    while ((token == PLUS) || (token == MINUS)) // + -
    {
        TreeNode * p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            t->child[1] = term();
        }
    }
    return t;
}

TreeNode * term(void)
{
    TreeNode * t = power_term();
    while ((token == TIMES) || (token == OVER) || (token == MOD)) // * / %
    {
        TreeNode * p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = power_term();
        }
    }
    return t;
}

/* 新增处理乘方 */
treeNode * power_term(void)
{
    TreeNode * t = factor();
    while (token == POWER) // ^
    {
        TreeNode * p = newExpNode(OpK);
        if (p != NULL)
        {
            p->child[0] = t;
            p->attr.op = token;
            t = p;
            match(token);
            p->child[1] = factor();
        }
    }
    return t;
}

TreeNode * factor(void)
{
    TreeNode * t = NULL;
    switch (token)
    {
    case NUM :
        t = newExpNode(ConstK);
        if ((t != NULL) && (token == NUM))
            t->attr.val = atoi(tokenString);
        match(NUM);
        break;
    case ID :
        t = newExpNode(IdK);
        if ((t!=NULL) && (token==ID))
            t->attr.name = copyString(tokenString);
        match(ID);
        break;
    case LPAREN :
        match(LPAREN);
        t = exp();
        match(RPAREN);
        break;
    default:
        syntaxError("unexpected token -> ");
        printToken(token, tokenString);
        token = getToken();
        break;
    }
    return t;
}

/* 扩充while文法 */
TreeNode * while_stmt(void)
{
    TreeNode * t = newStmtNode(WhileK);
    match(WHILE);
    if(t != NULL)
        t->child[0] = exp();
    match(DO);
    if(t != NULL)
        t->child[1] = stmt_sequence();
    match(ENDWHILE);
    return t;
}

/* 扩充dowhile文法 */
TreeNode * dowhile_stmt(void)
{
    TreeNode * t = newStmtNode(DowhileK);
    match(DO);
    if(t != NULL)
        t->child[0] = stmt_sequence();
    match(WHILE);
    match(LPAREN);
    if(t != NULL)
        t->child[1] = exp();
    match(RPAREN);
    return t;
}

/* 扩充for文法 */
TreeNode * for_stmt(void)
{
    TreeNode * t = newStmtNode(ForK);
    match(FOR);
    if(t != NULL)
        t->child[0] = assign_stmt();
    if(token == TO)
    {
        match(TO);
        if(t != NULL)
            t->child[1] = simple_exp();
    }
    else
    {
        match(DOWNTO);
        if(t != NULL)
            t->child[1] = simple_exp();
    }
    match(DO);
    if(t != NULL)
        t->child[2] = stmt_sequence();
    match(ENDDO);
    return t;
}

/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode * parse(void)
{
    TreeNode * t;
    token = getToken();
    t = stmt_sequence();
    if (token != ENDFILE)
        syntaxError("Code ends before file\n");
    return t;
}
