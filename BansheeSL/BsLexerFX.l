%{
#include "BsParserFX.h"

#define YY_USER_ACTION yylloc->first_column = yycolumn + 1; yylloc->first_line = yylineno + 1; yycolumn += (int)yyleng;
#define YY_USER_INIT yylineno = 0; yycolumn = 0;
%}
 
%option yylineno reentrant noyywrap nounistd never-interactive warn nodefault bison-bridge bison-locations
%option outfile="BsLexerFX.c" header-file="BsLexerFX.h"
%option extra-type="struct tagParseState *"

INTEGER			-?[0-9][0-9]*
INTEGER_16		0[xX][0-9a-fA-F]+
FLOAT			[0-9]+\.[0-9]+([eE][+-]?[0-9]+)?[fF]?
STRING			\"[^"\n]*\"
IDENTIFIER		[_a-zA-Z][_a-zA-Z0-9]*
WS				[ \r\n\t]*
COMMENT			\/\/[^\n]*

%%

{WS}			{ /* Skip blank */ }
{INTEGER}       { yylval->intValue = atoi(yytext); return TOKEN_INTEGER; }
{INTEGER_16}    { yylval->intValue = (int)strtol(yytext, 0, 0); return TOKEN_INTEGER; }
{FLOAT}			{ yylval->floatValue = (float)atof(yytext); return TOKEN_FLOAT; }
{STRING}		{ yylval->strValue = mmalloc_strdup(yyextra->memContext, yytext); return TOKEN_STRING; }
true			{ yylval->intValue = 1; return TOKEN_BOOLEAN; }
false			{ yylval->intValue = 0; return TOKEN_BOOLEAN; }

	/* Value types */

float			{ yylval->intValue = PT_Float; return TOKEN_FLOATTYPE; } 
float2			{ yylval->intValue = PT_Float2; return TOKEN_FLOAT2TYPE; } 
float3			{ yylval->intValue = PT_Float3; return TOKEN_FLOAT3TYPE; } 
float4			{ yylval->intValue = PT_Float4; return TOKEN_FLOAT4TYPE; }

mat2x2			{ yylval->intValue = PT_Mat2x2; return TOKEN_MAT2x2TYPE; } 
mat2x3			{ yylval->intValue = PT_Mat2x3; return TOKEN_MAT2x3TYPE; } 
mat2x4			{ yylval->intValue = PT_Mat2x4; return TOKEN_MAT2x4TYPE; }

mat3x2			{ yylval->intValue = PT_Mat3x2; return TOKEN_MAT3x2TYPE; } 
mat3x3			{ yylval->intValue = PT_Mat3x3; return TOKEN_MAT3x3TYPE; } 
mat3x4			{ yylval->intValue = PT_Mat3x4; return TOKEN_MAT3x4TYPE; }

mat4x2			{ yylval->intValue = PT_Mat4x2; return TOKEN_MAT4x2TYPE; } 
mat4x3			{ yylval->intValue = PT_Mat4x3; return TOKEN_MAT4x3TYPE; } 
mat4x4			{ yylval->intValue = PT_Mat4x4; return TOKEN_MAT4x4TYPE; }

Sampler1D		{ yylval->intValue = PT_Sampler1D; return TOKEN_SAMPLER1D; } 
Sampler2D		{ yylval->intValue = PT_Sampler2D; return TOKEN_SAMPLER2D; } 
Sampler3D		{ yylval->intValue = PT_Sampler3D; return TOKEN_SAMPLER3D; } 
SamplerCUBE		{ yylval->intValue = PT_SamplerCUBE; return TOKEN_SAMPLERCUBE; } 
Sampler2DMS		{ yylval->intValue = PT_Sampler2DMS; return TOKEN_SAMPLER2DMS; }

Texture1D		{ yylval->intValue = PT_Texture1D; return TOKEN_TEXTURE1D; } 
Texture2D		{ yylval->intValue = PT_Texture2D; return TOKEN_TEXTURE2D; } 
Texture3D		{ yylval->intValue = PT_Texture3D; return TOKEN_TEXTURE3D; } 
TextureCUBE		{ yylval->intValue = PT_TextureCUBE; return TOKEN_TEXTURECUBE; } 
Texture2DMS		{ yylval->intValue = PT_Texture2DMS; return TOKEN_TEXTURE2DMS; }

ByteBuffer		{ yylval->intValue = PT_ByteBuffer; return TOKEN_BYTEBUFFER; } 
StructBuffer	{ yylval->intValue = PT_StructBuffer; return TOKEN_STRUCTBUFFER; } 

TypedBufferRW	{ yylval->intValue = PT_TypedBufferRW; return TOKEN_RWTYPEDBUFFER; } 
ByteBufferRW	{ yylval->intValue = PT_ByteBufferRW; return TOKEN_RWBYTEBUFFER; }
StructBufferRW	{ yylval->intValue = PT_StructBufferRW; return TOKEN_RWSTRUCTBUFFER; } 
AppendBuffer	{ yylval->intValue = PT_AppendBuffer; return TOKEN_RWAPPENDBUFFER; } 
ConsumeBuffer	{ yylval->intValue = PT_ConsumeBuffer; return TOKEN_RWCONSUMEBUFFER; }

Block			{ return TOKEN_PARAMSBLOCK; }

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
StencilRef		{ return TOKEN_STENCILREF; }

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

	/* Qualifiers */
auto				{ return TOKEN_AUTO; }
alias				{ return TOKEN_ALIAS; }
shared				{ return TOKEN_SHARED; }
usage				{ return TOKEN_USAGE; }

	/* State values */
WIRE			{ yylval->intValue = FMV_Wire; return TOKEN_FILLMODEVALUE; }
SOLID			{ yylval->intValue = FMV_Solid; return TOKEN_FILLMODEVALUE; }

NOCULL			{ yylval->intValue = CMV_None; return TOKEN_CULLMODEVALUE; }
CW				{ yylval->intValue = CMV_CW; return TOKEN_CULLMODEVALUE; }
CCW				{ yylval->intValue = CMV_CCW; return TOKEN_CULLMODEVALUE; }

FAIL			{ yylval->intValue = CFV_Fail; return TOKEN_COMPFUNCVALUE; }
PASS			{ yylval->intValue = CFV_Pass; return TOKEN_COMPFUNCVALUE; }
LT				{ yylval->intValue = CFV_LT; return TOKEN_COMPFUNCVALUE; }
LTE				{ yylval->intValue = CFV_LTE; return TOKEN_COMPFUNCVALUE; }
EQ				{ yylval->intValue = CFV_EQ; return TOKEN_COMPFUNCVALUE; }
NEQ				{ yylval->intValue = CFV_NEQ; return TOKEN_COMPFUNCVALUE; }
GTE				{ yylval->intValue = CFV_GTE; return TOKEN_COMPFUNCVALUE; }
GT				{ yylval->intValue = CFV_GT; return TOKEN_COMPFUNCVALUE; }

KEEP				{ yylval->intValue = OV_Keep; return TOKEN_OPVALUE; }
ZERO				{ yylval->intValue = OV_Zero; return TOKEN_OPVALUE; }
REPLACE				{ yylval->intValue = OV_Replace; return TOKEN_OPVALUE; }
INC					{ yylval->intValue = OV_Incr; return TOKEN_OPVALUE; }
DEC					{ yylval->intValue = OV_Decr; return TOKEN_OPVALUE; }
INCWRAP				{ yylval->intValue = OV_IncrWrap; return TOKEN_OPVALUE; }
DECWRAP				{ yylval->intValue = OV_DecrWrap; return TOKEN_OPVALUE; }
INV					{ yylval->intValue = OV_Invert; return TOKEN_OPVALUE; }
ONE					{ yylval->intValue = OV_One; return TOKEN_OPVALUE; }
DSTRGB				{ yylval->intValue = OV_DestColor; return TOKEN_OPVALUE; }
SRCRGB				{ yylval->intValue = OV_SrcColor; return TOKEN_OPVALUE; }
DSTIRGB				{ yylval->intValue = OV_InvDestColor; return TOKEN_OPVALUE; }
SRCIRGB				{ yylval->intValue = OV_InvSrcColor; return TOKEN_OPVALUE; }
DSTA				{ yylval->intValue = OV_DestAlpha; return TOKEN_OPVALUE; }
SRCA				{ yylval->intValue = OV_SrcAlpha; return TOKEN_OPVALUE; }
DSTIA				{ yylval->intValue = OV_InvDestAlpha; return TOKEN_OPVALUE; }
SRCIA				{ yylval->intValue = OV_InvSrcAlpha; return TOKEN_OPVALUE; }

ADD					{ yylval->intValue = BOV_Add; return TOKEN_BLENDOPVALUE; }
SUB					{ yylval->intValue = BOV_Subtract; return TOKEN_BLENDOPVALUE; }
RSUB				{ yylval->intValue = BOV_RevSubtract; return TOKEN_BLENDOPVALUE; }
MIN					{ yylval->intValue = BOV_Min; return TOKEN_BLENDOPVALUE; }
MAX					{ yylval->intValue = BOV_Max; return TOKEN_BLENDOPVALUE; }

NOCOLOR			{ yylval->intValue = 0x0; return TOKEN_COLORMASK; }
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

WRAP			{ yylval->intValue = AMV_Wrap; return TOKEN_ADDRMODEVALUE; }
MIRROR			{ yylval->intValue = AMV_Mirror; return TOKEN_ADDRMODEVALUE; }
CLAMP			{ yylval->intValue = AMV_Clamp; return TOKEN_ADDRMODEVALUE; }
BORDER			{ yylval->intValue = AMV_Border; return TOKEN_ADDRMODEVALUE; }

NOFILTER		{ yylval->intValue = FV_None; return TOKEN_FILTERVALUE; }
POINT			{ yylval->intValue = FV_Point; return TOKEN_FILTERVALUE; }
LINEAR			{ yylval->intValue = FV_Linear; return TOKEN_FILTERVALUE; }
ANISO			{ yylval->intValue = FV_Anisotropic; return TOKEN_FILTERVALUE; }
POINTC			{ yylval->intValue = FV_PointCmp; return TOKEN_FILTERVALUE; }
LINEARC			{ yylval->intValue = FV_LinearCmp; return TOKEN_FILTERVALUE; }
ANISOC			{ yylval->intValue = FV_AnisotropicCmp; return TOKEN_FILTERVALUE; }

STATIC			{ yylval->intValue = BUV_Static; return TOKEN_BUFFERUSAGE; }
DYNAMIC			{ yylval->intValue = BUV_Dynamic; return TOKEN_BUFFERUSAGE; }

{COMMENT}		{ }
{IDENTIFIER}	{ yylval->strValue = mmalloc_strdup(yyextra->memContext, yytext); return TOKEN_IDENTIFIER; }
.				{ return yytext[0]; }
