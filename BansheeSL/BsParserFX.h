/* A Bison parser, made by GNU Bison 2.7.  */

/* Skeleton interface for Bison GLR parsers in C
   
      Copyright (C) 2002-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

#ifndef YY_YY_BSPARSERFX_H_INCLUDED
# define YY_YY_BSPARSERFX_H_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 2579 of glr.c  */
#line 13 "BsParserFX.y"

#include "BsMMAlloc.h"
#include "BsASTFX.h"

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
#endif



/* Line 2579 of glr.c  */
#line 58 "BsParserFX.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TOKEN_INTEGER = 258,
     TOKEN_FLOAT = 259,
     TOKEN_BOOLEAN = 260,
     TOKEN_STRING = 261,
     TOKEN_FILLMODEVALUE = 262,
     TOKEN_CULLMODEVALUE = 263,
     TOKEN_COMPFUNCVALUE = 264,
     TOKEN_OPVALUE = 265,
     TOKEN_COLORMASK = 266,
     TOKEN_ADDRMODEVALUE = 267,
     TOKEN_FILTERVALUE = 268,
     TOKEN_BLENDOPVALUE = 269,
     TOKEN_SEPARABLE = 270,
     TOKEN_QUEUE = 271,
     TOKEN_PRIORITY = 272,
     TOKEN_PARAMETERS = 273,
     TOKEN_BLOCKS = 274,
     TOKEN_TECHNIQUE = 275,
     TOKEN_RENDERER = 276,
     TOKEN_LANGUAGE = 277,
     TOKEN_INCLUDE = 278,
     TOKEN_PASS = 279,
     TOKEN_VERTEX = 280,
     TOKEN_FRAGMENT = 281,
     TOKEN_GEOMETRY = 282,
     TOKEN_HULL = 283,
     TOKEN_DOMAIN = 284,
     TOKEN_COMPUTE = 285,
     TOKEN_FILLMODE = 286,
     TOKEN_CULLMODE = 287,
     TOKEN_DEPTHBIAS = 288,
     TOKEN_SDEPTHBIAS = 289,
     TOKEN_DEPTHCLIP = 290,
     TOKEN_SCISSOR = 291,
     TOKEN_MULTISAMPLE = 292,
     TOKEN_AALINE = 293,
     TOKEN_DEPTHREAD = 294,
     TOKEN_DEPTHWRITE = 295,
     TOKEN_COMPAREFUNC = 296,
     TOKEN_STENCIL = 297,
     TOKEN_STENCILREADMASK = 298,
     TOKEN_STENCILWRITEMASK = 299,
     TOKEN_STENCILOPFRONT = 300,
     TOKEN_STENCILOPBACK = 301,
     TOKEN_FAIL = 302,
     TOKEN_ZFAIL = 303,
     TOKEN_ALPHATOCOVERAGE = 304,
     TOKEN_INDEPENDANTBLEND = 305,
     TOKEN_TARGET = 306,
     TOKEN_INDEX = 307,
     TOKEN_BLEND = 308,
     TOKEN_COLOR = 309,
     TOKEN_ALPHA = 310,
     TOKEN_WRITEMASK = 311,
     TOKEN_SOURCE = 312,
     TOKEN_DEST = 313,
     TOKEN_OP = 314,
     TOKEN_ADDRMODE = 315,
     TOKEN_MINFILTER = 316,
     TOKEN_MAGFILTER = 317,
     TOKEN_MIPFILTER = 318,
     TOKEN_MAXANISO = 319,
     TOKEN_MIPBIAS = 320,
     TOKEN_MIPMIN = 321,
     TOKEN_MIPMAX = 322,
     TOKEN_BORDERCOLOR = 323,
     TOKEN_U = 324,
     TOKEN_V = 325,
     TOKEN_W = 326
   };
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2579 of glr.c  */
#line 34 "BsParserFX.y"

	int intValue;
	float floatValue;
	const char* strValue;
	ASTFXNode* nodePtr;
	NodeOption nodeOption;


/* Line 2579 of glr.c  */
#line 152 "BsParserFX.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


int yyparse (ParseState* parse_state, yyscan_t scanner);

#endif /* !YY_YY_BSPARSERFX_H_INCLUDED  */
