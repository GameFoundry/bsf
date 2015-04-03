%{
#include "BsParserFX.h"
%}
 
%option reentrant noyywrap nounistd never-interactive warn nodefault bison-bridge
%option outfile="BsLexerFX.c" header-file="BsLexerFX.h"
%option extra-type="struct tagParseState *"
%option debug

LBRACKET		"{"
RBRACKET		"}"
 
INTEGER		[0-9][0-9]*
INTEGER_BIN	0x[0-9][0-9]*
FLOAT		[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fF]?
STRING		\"[^"\n]*\"
WS			[ \r\n\t]*

%%

{WS}			{ /* Skip blank */ }
{INTEGER}       { yylval->intValue = atoi(yytext); return TOKEN_INTEGER; }
{FLOAT}			{ yylval->floatValue = (float)atof(yytext); return TOKEN_FLOAT; }
{STRING}		{ yylval->strValue = mmalloc_strdup(yyextra->memContext, yytext); return TOKEN_STRING; }
true			{ yylval->intValue = 1; return TOKEN_BOOLEAN; }
false			{ yylval->intValue = 0; return TOKEN_BOOLEAN; }

{LBRACKET}      { return TOKEN_LBRACKET; }
{RBRACKET}      { return TOKEN_RBRACKET; }

	/* Shader keywords */
Separable		{ return TOKEN_SEPARABLE; }
Queue			{ return TOKEN_QUEUE; }
Priority		{ return TOKEN_PRIORITY; }

	/* Rasterizer state keywords */
Fill			{ return TOKEN_FILLMODE; }
Cull			{ return TOKEN_CULLMODE; }
DepthBias		{ return TOKEN_DEPTHBIAS; }
ScaledDepthBias	{ return TOKEN_SDEPTHBIAS; }
DepthClip		{ return TOKEN_DEPTHCLIP; }
Scissor			{ return TOKEN_SCISSOR; }
Multisample		{ return TOKEN_MULTISAMPLE; }
AALine			{ return TOKEN_AALINE; }

.				{ return yytext[0]; }
