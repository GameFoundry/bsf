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
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
/* Line 2579 of glr.c  */
#line 9 "BsParserFX.y"

#include "BsMMAlloc.h"
#include "BsASTFX.h"

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
#endif

#define ADD_PARAMETER(OUTPUT, TYPE, NAME)															\
			OUTPUT = nodeCreate(parse_state->memContext, NT_Parameter);								\
			nodePush(parse_state, OUTPUT);															\
																									\
			NodeOption paramType;																	\
			paramType.type = OT_ParamType;															\
			paramType.value.intValue = TYPE;														\
																									\
			NodeOption paramName;																	\
			paramName.type = OT_Identifier;															\
			paramName.value.strValue = NAME;														\
																									\
			nodeOptionsAdd(parse_state->memContext, parse_state->topNode->options, &paramType);		\
			nodeOptionsAdd(parse_state->memContext, parse_state->topNode->options, &paramName);		\



/* Line 2579 of glr.c  */
#line 73 "BsParserFX.h"

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
     TOKEN_IDENTIFIER = 262,
     TOKEN_FILLMODEVALUE = 263,
     TOKEN_CULLMODEVALUE = 264,
     TOKEN_COMPFUNCVALUE = 265,
     TOKEN_OPVALUE = 266,
     TOKEN_COLORMASK = 267,
     TOKEN_ADDRMODEVALUE = 268,
     TOKEN_FILTERVALUE = 269,
     TOKEN_BLENDOPVALUE = 270,
     TOKEN_BUFFERUSAGE = 271,
     TOKEN_FLOATTYPE = 272,
     TOKEN_FLOAT2TYPE = 273,
     TOKEN_FLOAT3TYPE = 274,
     TOKEN_FLOAT4TYPE = 275,
     TOKEN_MAT2x2TYPE = 276,
     TOKEN_MAT2x3TYPE = 277,
     TOKEN_MAT2x4TYPE = 278,
     TOKEN_MAT3x2TYPE = 279,
     TOKEN_MAT3x3TYPE = 280,
     TOKEN_MAT3x4TYPE = 281,
     TOKEN_MAT4x2TYPE = 282,
     TOKEN_MAT4x3TYPE = 283,
     TOKEN_MAT4x4TYPE = 284,
     TOKEN_SAMPLER1D = 285,
     TOKEN_SAMPLER2D = 286,
     TOKEN_SAMPLER3D = 287,
     TOKEN_SAMPLERCUBE = 288,
     TOKEN_SAMPLER2DMS = 289,
     TOKEN_TEXTURE1D = 290,
     TOKEN_TEXTURE2D = 291,
     TOKEN_TEXTURE3D = 292,
     TOKEN_TEXTURECUBE = 293,
     TOKEN_TEXTURE2DMS = 294,
     TOKEN_BYTEBUFFER = 295,
     TOKEN_STRUCTBUFFER = 296,
     TOKEN_RWTYPEDBUFFER = 297,
     TOKEN_RWBYTEBUFFER = 298,
     TOKEN_RWSTRUCTBUFFER = 299,
     TOKEN_RWAPPENDBUFFER = 300,
     TOKEN_RWCONSUMEBUFFER = 301,
     TOKEN_PARAMSBLOCK = 302,
     TOKEN_AUTO = 303,
     TOKEN_ALIAS = 304,
     TOKEN_SHARED = 305,
     TOKEN_USAGE = 306,
     TOKEN_SEPARABLE = 307,
     TOKEN_QUEUE = 308,
     TOKEN_PRIORITY = 309,
     TOKEN_PARAMETERS = 310,
     TOKEN_BLOCKS = 311,
     TOKEN_TECHNIQUE = 312,
     TOKEN_RENDERER = 313,
     TOKEN_LANGUAGE = 314,
     TOKEN_INCLUDE = 315,
     TOKEN_PASS = 316,
     TOKEN_VERTEX = 317,
     TOKEN_FRAGMENT = 318,
     TOKEN_GEOMETRY = 319,
     TOKEN_HULL = 320,
     TOKEN_DOMAIN = 321,
     TOKEN_COMPUTE = 322,
     TOKEN_STENCILREF = 323,
     TOKEN_FILLMODE = 324,
     TOKEN_CULLMODE = 325,
     TOKEN_DEPTHBIAS = 326,
     TOKEN_SDEPTHBIAS = 327,
     TOKEN_DEPTHCLIP = 328,
     TOKEN_SCISSOR = 329,
     TOKEN_MULTISAMPLE = 330,
     TOKEN_AALINE = 331,
     TOKEN_DEPTHREAD = 332,
     TOKEN_DEPTHWRITE = 333,
     TOKEN_COMPAREFUNC = 334,
     TOKEN_STENCIL = 335,
     TOKEN_STENCILREADMASK = 336,
     TOKEN_STENCILWRITEMASK = 337,
     TOKEN_STENCILOPFRONT = 338,
     TOKEN_STENCILOPBACK = 339,
     TOKEN_FAIL = 340,
     TOKEN_ZFAIL = 341,
     TOKEN_ALPHATOCOVERAGE = 342,
     TOKEN_INDEPENDANTBLEND = 343,
     TOKEN_TARGET = 344,
     TOKEN_INDEX = 345,
     TOKEN_BLEND = 346,
     TOKEN_COLOR = 347,
     TOKEN_ALPHA = 348,
     TOKEN_WRITEMASK = 349,
     TOKEN_SOURCE = 350,
     TOKEN_DEST = 351,
     TOKEN_OP = 352,
     TOKEN_ADDRMODE = 353,
     TOKEN_MINFILTER = 354,
     TOKEN_MAGFILTER = 355,
     TOKEN_MIPFILTER = 356,
     TOKEN_MAXANISO = 357,
     TOKEN_MIPBIAS = 358,
     TOKEN_MIPMIN = 359,
     TOKEN_MIPMAX = 360,
     TOKEN_BORDERCOLOR = 361,
     TOKEN_U = 362,
     TOKEN_V = 363,
     TOKEN_W = 364
   };
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2579 of glr.c  */
#line 45 "BsParserFX.y"

	int intValue;
	float floatValue;
	float matrixValue[16];
	const char* strValue;
	ASTFXNode* nodePtr;
	NodeOption nodeOption;


/* Line 2579 of glr.c  */
#line 206 "BsParserFX.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


int yyparse (ParseState* parse_state, yyscan_t scanner);

#endif /* !YY_YY_BSPARSERFX_H_INCLUDED  */
