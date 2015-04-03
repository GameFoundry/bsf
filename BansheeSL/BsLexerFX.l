%{
#include "BsParserFX.h"
%}
 
%option reentrant noyywrap nounistd never-interactive warn nodefault bison-bridge
%option outfile="BsLexerFX.c" header-file="BsLexerFX.h"
%option extra-type="struct tagParseState *"
%option debug

INTEGER			[0-9][0-9]*
INTEGER_BIN		0x[0-9][0-9]*
FLOAT			[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fF]?
STRING			\"[^"\n]*\"
WS				[ \r\n\t]*

%%

{WS}			{ /* Skip blank */ }
{INTEGER}       { yylval->intValue = atoi(yytext); return TOKEN_INTEGER; }
{FLOAT}			{ yylval->floatValue = (float)atof(yytext); return TOKEN_FLOAT; }
{STRING}		{ yylval->strValue = mmalloc_strdup(yyextra->memContext, yytext); return TOKEN_STRING; }
true			{ yylval->intValue = 1; return TOKEN_BOOLEAN; }
false			{ yylval->intValue = 0; return TOKEN_BOOLEAN; }

	/* Shader keywords */
Separable		{ return TOKEN_SEPARABLE; }
Queue			{ return TOKEN_QUEUE; }
Priority		{ return TOKEN_PRIORITY; }
Technique		{ return TOKEN_TECHNIQUE; }
Parameters		{ return TOKEN_PARAMETERS; }
Blocks			{ return TOKEN_BLOCKS; }

	/* Technique keywords */
Renderer		{ return TOKEN_RENDERER; }
Language		{ return TOKEN_LANGUAGE; }
Include			{ return TOKEN_INCLUDE; }
Pass			{ return TOKEN_PASS; }

	/* Pass keywords */
Vertex			{ return TOKEN_VERTEX; }
Fragment		{ return TOKEN_FRAGMENT; }
Geometry		{ return TOKEN_GEOMETRY; }
Hull			{ return TOKEN_HULL; }
Domain			{ return TOKEN_DOMAIN; }
Compute			{ return TOKEN_COMPUTE; }

	/* Rasterizer state keywords */
Fill			{ return TOKEN_FILLMODE; }
Cull			{ return TOKEN_CULLMODE; }
DepthBias		{ return TOKEN_DEPTHBIAS; }
ScaledDepthBias	{ return TOKEN_SDEPTHBIAS; }
DepthClip		{ return TOKEN_DEPTHCLIP; }
Scissor			{ return TOKEN_SCISSOR; }
Multisample		{ return TOKEN_MULTISAMPLE; }
AALine			{ return TOKEN_AALINE; }

	/* Depth-stencil state keywords */
DepthRead			{ return TOKEN_DEPTHREAD; }
DepthWrite			{ return TOKEN_DEPTHWRITE; }
CompareFunc			{ return TOKEN_COMPAREFUNC; }
Stencil				{ return TOKEN_STENCIL; }
StencilReadMask		{ return TOKEN_STENCILREADMASK; }
StencilWriteMask	{ return TOKEN_STENCILWRITEMASK; }
StencilOpFront		{ return TOKEN_STENCILOPFRONT; }
StencilOpBack		{ return TOKEN_STENCILOPBACK; }
Fail				{ return TOKEN_FAIL; }
ZFail				{ return TOKEN_ZFAIL; }

	/* Blend state keywords */
AlphaToCoverage		{ return TOKEN_ALPHATOCOVERAGE; }
IndependantBlend	{ return TOKEN_INDEPENDANTBLEND; }
Target				{ return TOKEN_TARGET; }
Index				{ return TOKEN_INDEX; }
Blend				{ return TOKEN_BLEND; }
Color				{ return TOKEN_COLOR; }
Alpha				{ return TOKEN_ALPHA; }
WriteMask			{ return TOKEN_WRITEMASK; }
Source				{ return TOKEN_SOURCE; }
Dest				{ return TOKEN_DEST; }
Op					{ return TOKEN_OP; }

	/* Sampler state keywords */
AddressMode			{ return TOKEN_ADDRMODE; }
MinFilter			{ return TOKEN_MINFILTER; }
MagFilter			{ return TOKEN_MAGFILTER; }
MipFilter			{ return TOKEN_MIPFILTER; }
MaxAniso			{ return TOKEN_MAXANISO; }
MipmapBias			{ return TOKEN_MIPBIAS; }
MipMin				{ return TOKEN_MIPMIN; }
MipMax				{ return TOKEN_MIPMAX; }
BorderColor			{ return TOKEN_BORDERCOLOR; }
U					{ return TOKEN_U; }
V					{ return TOKEN_V; }
W					{ return TOKEN_W; }

	/* State values */
wire			{ yylval->intValue = 0; return TOKEN_FILLMODEVALUE; }
solid			{ yylval->intValue = 1; return TOKEN_FILLMODEVALUE; }

nocull			{ yylval->intValue = 0; return TOKEN_CULLMODEVALUE; }
cw				{ yylval->intValue = 1; return TOKEN_CULLMODEVALUE; }
ccw				{ yylval->intValue = 2; return TOKEN_CULLMODEVALUE; }

fail			{ yylval->intValue = 0; return TOKEN_COMPFUNCVALUE; }
pass			{ yylval->intValue = 1; return TOKEN_COMPFUNCVALUE; }
lt				{ yylval->intValue = 2; return TOKEN_COMPFUNCVALUE; }
lte				{ yylval->intValue = 3; return TOKEN_COMPFUNCVALUE; }
eq				{ yylval->intValue = 4; return TOKEN_COMPFUNCVALUE; }
neq				{ yylval->intValue = 5; return TOKEN_COMPFUNCVALUE; }
gte				{ yylval->intValue = 6; return TOKEN_COMPFUNCVALUE; }
gt				{ yylval->intValue = 7; return TOKEN_COMPFUNCVALUE; }

keep			{ yylval->intValue = 0; return TOKEN_OPVALUE; }
zero			{ yylval->intValue = 1; return TOKEN_OPVALUE; }
replace			{ yylval->intValue = 2; return TOKEN_OPVALUE; }
incr			{ yylval->intValue = 3; return TOKEN_OPVALUE; }
decr			{ yylval->intValue = 4; return TOKEN_OPVALUE; }
incrwrap		{ yylval->intValue = 5; return TOKEN_OPVALUE; }
decrwrap		{ yylval->intValue = 6; return TOKEN_OPVALUE; }
invert			{ yylval->intValue = 7; return TOKEN_OPVALUE; }
one				{ yylval->intValue = 8; return TOKEN_OPVALUE; }
destcolor		{ yylval->intValue = 9; return TOKEN_OPVALUE; }
srccolor		{ yylval->intValue = 10; return TOKEN_OPVALUE; }
invdestcolor	{ yylval->intValue = 11; return TOKEN_OPVALUE; }
invsrccolor		{ yylval->intValue = 12; return TOKEN_OPVALUE; }
destalpha		{ yylval->intValue = 13; return TOKEN_OPVALUE; }
srcalpha		{ yylval->intValue = 14; return TOKEN_OPVALUE; }
invdestalpha	{ yylval->intValue = 15; return TOKEN_OPVALUE; }
invsrcalpha		{ yylval->intValue = 16; return TOKEN_OPVALUE; }

add				{ yylval->intValue = 0; return TOKEN_BLENDOPVALUE; }
subtract		{ yylval->intValue = 1; return TOKEN_BLENDOPVALUE; }
revsubtract		{ yylval->intValue = 2; return TOKEN_BLENDOPVALUE; }
min				{ yylval->intValue = 3; return TOKEN_BLENDOPVALUE; }
max				{ yylval->intValue = 4; return TOKEN_BLENDOPVALUE; }

nocolor			{ yylval->intValue = 0x0; return TOKEN_COLORMASK; }
R				{ yylval->intValue = 0x1; return TOKEN_COLORMASK; }
G				{ yylval->intValue = 0x2; return TOKEN_COLORMASK; }
B				{ yylval->intValue = 0x4; return TOKEN_COLORMASK; }
A				{ yylval->intValue = 0x8; return TOKEN_COLORMASK; }
RG				{ yylval->intValue = 0x3; return TOKEN_COLORMASK; }
RB				{ yylval->intValue = 0x5; return TOKEN_COLORMASK; }
RA				{ yylval->intValue = 0x9; return TOKEN_COLORMASK; }
GB				{ yylval->intValue = 0x6; return TOKEN_COLORMASK; }
GA				{ yylval->intValue = 0xA; return TOKEN_COLORMASK; }
BA				{ yylval->intValue = 0xC; return TOKEN_COLORMASK; }
RGB				{ yylval->intValue = 0x7; return TOKEN_COLORMASK; }
RGA				{ yylval->intValue = 0xB; return TOKEN_COLORMASK; }
RBA				{ yylval->intValue = 0xD; return TOKEN_COLORMASK; }
GBA				{ yylval->intValue = 0xE; return TOKEN_COLORMASK; }
RGBA			{ yylval->intValue = 0xF; return TOKEN_COLORMASK; }

wrap			{ yylval->intValue = 0; return TOKEN_ADDRMODEVALUE; }
mirror			{ yylval->intValue = 1; return TOKEN_ADDRMODEVALUE; }
clamp			{ yylval->intValue = 2; return TOKEN_ADDRMODEVALUE; }
border			{ yylval->intValue = 3; return TOKEN_ADDRMODEVALUE; }

nofilter		{ yylval->intValue = 0; return TOKEN_FILTERVALUE; }
point			{ yylval->intValue = 1; return TOKEN_FILTERVALUE; }
linear			{ yylval->intValue = 2; return TOKEN_FILTERVALUE; }
anisotropic		{ yylval->intValue = 3; return TOKEN_FILTERVALUE; }
pointcmp		{ yylval->intValue = 4; return TOKEN_FILTERVALUE; }
linearcmp		{ yylval->intValue = 5; return TOKEN_FILTERVALUE; }
anisotropiccmp	{ yylval->intValue = 6; return TOKEN_FILTERVALUE; }


.				{ return yytext[0]; }
