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
     TOKEN_INTTYPE = 276,
     TOKEN_INT2TYPE = 277,
     TOKEN_INT3TYPE = 278,
     TOKEN_INT4TYPE = 279,
     TOKEN_COLORTYPE = 280,
     TOKEN_MAT2x2TYPE = 281,
     TOKEN_MAT2x3TYPE = 282,
     TOKEN_MAT2x4TYPE = 283,
     TOKEN_MAT3x2TYPE = 284,
     TOKEN_MAT3x3TYPE = 285,
     TOKEN_MAT3x4TYPE = 286,
     TOKEN_MAT4x2TYPE = 287,
     TOKEN_MAT4x3TYPE = 288,
     TOKEN_MAT4x4TYPE = 289,
     TOKEN_SAMPLER1D = 290,
     TOKEN_SAMPLER2D = 291,
     TOKEN_SAMPLER3D = 292,
     TOKEN_SAMPLERCUBE = 293,
     TOKEN_SAMPLER2DMS = 294,
     TOKEN_TEXTURE1D = 295,
     TOKEN_TEXTURE2D = 296,
     TOKEN_TEXTURE3D = 297,
     TOKEN_TEXTURECUBE = 298,
     TOKEN_TEXTURE2DMS = 299,
     TOKEN_BYTEBUFFER = 300,
     TOKEN_STRUCTBUFFER = 301,
     TOKEN_RWTYPEDBUFFER = 302,
     TOKEN_RWBYTEBUFFER = 303,
     TOKEN_RWSTRUCTBUFFER = 304,
     TOKEN_RWAPPENDBUFFER = 305,
     TOKEN_RWCONSUMEBUFFER = 306,
     TOKEN_PARAMSBLOCK = 307,
     TOKEN_AUTO = 308,
     TOKEN_ALIAS = 309,
     TOKEN_SHARED = 310,
     TOKEN_USAGE = 311,
     TOKEN_SEPARABLE = 312,
     TOKEN_QUEUE = 313,
     TOKEN_PRIORITY = 314,
     TOKEN_TRANSPARENT = 315,
     TOKEN_PARAMETERS = 316,
     TOKEN_BLOCKS = 317,
     TOKEN_TECHNIQUE = 318,
     TOKEN_RENDERER = 319,
     TOKEN_LANGUAGE = 320,
     TOKEN_INCLUDE = 321,
     TOKEN_PASS = 322,
     TOKEN_VERTEX = 323,
     TOKEN_FRAGMENT = 324,
     TOKEN_GEOMETRY = 325,
     TOKEN_HULL = 326,
     TOKEN_DOMAIN = 327,
     TOKEN_COMPUTE = 328,
     TOKEN_COMMON = 329,
     TOKEN_STENCILREF = 330,
     TOKEN_FILLMODE = 331,
     TOKEN_CULLMODE = 332,
     TOKEN_DEPTHBIAS = 333,
     TOKEN_SDEPTHBIAS = 334,
     TOKEN_DEPTHCLIP = 335,
     TOKEN_SCISSOR = 336,
     TOKEN_MULTISAMPLE = 337,
     TOKEN_AALINE = 338,
     TOKEN_DEPTHREAD = 339,
     TOKEN_DEPTHWRITE = 340,
     TOKEN_COMPAREFUNC = 341,
     TOKEN_STENCIL = 342,
     TOKEN_STENCILREADMASK = 343,
     TOKEN_STENCILWRITEMASK = 344,
     TOKEN_STENCILOPFRONT = 345,
     TOKEN_STENCILOPBACK = 346,
     TOKEN_FAIL = 347,
     TOKEN_ZFAIL = 348,
     TOKEN_ALPHATOCOVERAGE = 349,
     TOKEN_INDEPENDANTBLEND = 350,
     TOKEN_TARGET = 351,
     TOKEN_INDEX = 352,
     TOKEN_BLEND = 353,
     TOKEN_COLOR = 354,
     TOKEN_ALPHA = 355,
     TOKEN_WRITEMASK = 356,
     TOKEN_SOURCE = 357,
     TOKEN_DEST = 358,
     TOKEN_OP = 359,
     TOKEN_ADDRMODE = 360,
     TOKEN_MINFILTER = 361,
     TOKEN_MAGFILTER = 362,
     TOKEN_MIPFILTER = 363,
     TOKEN_MAXANISO = 364,
     TOKEN_MIPBIAS = 365,
     TOKEN_MIPMIN = 366,
     TOKEN_MIPMAX = 367,
     TOKEN_BORDERCOLOR = 368,
     TOKEN_U = 369,
     TOKEN_V = 370,
     TOKEN_W = 371
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
	int intVectorValue[4];
	const char* strValue;
	ASTFXNode* nodePtr;
	NodeOption nodeOption;


/* Line 2579 of glr.c  */
#line 214 "BsParserFX.h"
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
