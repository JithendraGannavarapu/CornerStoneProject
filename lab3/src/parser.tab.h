/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENTIFIER = 258,
     INTEGER = 259,
     VAR = 260,
     IF = 261,
     ELSE = 262,
     WHILE = 263,
     ASSIGN = 264,
     PLUS = 265,
     MINUS = 266,
     MULT = 267,
     DIV = 268,
     EQ = 269,
     NEQ = 270,
     LT = 271,
     GT = 272,
     LE = 273,
     GE = 274,
     SEMI = 275,
     LPAREN = 276,
     RPAREN = 277,
     LBRACE = 278,
     RBRACE = 279
   };
#endif
/* Tokens.  */
#define IDENTIFIER 258
#define INTEGER 259
#define VAR 260
#define IF 261
#define ELSE 262
#define WHILE 263
#define ASSIGN 264
#define PLUS 265
#define MINUS 266
#define MULT 267
#define DIV 268
#define EQ 269
#define NEQ 270
#define LT 271
#define GT 272
#define LE 273
#define GE 274
#define SEMI 275
#define LPAREN 276
#define RPAREN 277
#define LBRACE 278
#define RBRACE 279




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 48 "src/parser.y"
{
    int ival;           
    char* sval;         
    struct ASTNode* nval; 
}
/* Line 1529 of yacc.c.  */
#line 103 "src/parser.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

