%{
#include "BsParserFX.h"
%}
 
%option reentrant noyywrap
%option nounistd
%option never-interactive
%option outfile="BsLexerFX.c" header-file="BsLexerFX.h"
%option warn nodefault
%option debug

%option bison-bridge
 
LBRACKET		"{"
RBRACKET		"}"
 
INTEGER		[0-9][0-9]*
INTEGER_BIN	0x[0-9][0-9]*
FLOAT		[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fF]?
BOOLEAN		(true|false)
STRING		\"[^"\n]*\"
WS			[ \r\n\t]*

%%

{WS}			{ /* Skip blank */ }
{INTEGER}       { return TOKEN_INTEGER; }
{FLOAT}			{ return TOKEN_FLOAT; }
{BOOLEAN}		{ return TOKEN_BOOLEAN; }
{STRING}		{ return TOKEN_STRING; }
 
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
