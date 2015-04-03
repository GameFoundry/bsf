%{
#include "BsParserFX.h"
#include "BsLexerFX.h"
#define inline

	void yyerror(ParseState* parse_state, yyscan_t scanner, const char *msg) 
	{ 
		fprintf (stderr, "%s\n", msg);
	}

%}

%code requires{
#include "BsMMAlloc.h"
#include "BsASTFX.h"

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
#endif

}

%output  "BsParserFX.c"
%defines "BsParserFX.h"

%define api.pure
%debug
%lex-param { yyscan_t scanner }
%parse-param { ParseState* parse_state }
%parse-param { yyscan_t scanner }
%glr-parser

%union {
	int intValue;
	float floatValue;
	const char* strValue;
	ASTFXNode* nodePtr;
	NodeOption nodeOption;
}

	/* Value types */
%token <intValue>	TOKEN_INTEGER
%token <floatValue> TOKEN_FLOAT
%token <intValue>	TOKEN_BOOLEAN
%token <strValue>	TOKEN_STRING

%token <intValue>	TOKEN_FILLMODEVALUE
%token <intValue>	TOKEN_CULLMODEVALUE
%token <intValue>	TOKEN_COMPFUNCVALUE
%token <intValue>	TOKEN_OPVALUE
%token <intValue>	TOKEN_COLORMASK
%token <intValue>	TOKEN_ADDRMODEVALUE
%token <intValue>	TOKEN_FILTERVALUE
%token <intValue>	TOKEN_BLENDOPVALUE

	/* Shader keywords */
%token TOKEN_SEPARABLE TOKEN_QUEUE TOKEN_PRIORITY
%token TOKEN_PARAMETERS TOKEN_BLOCKS TOKEN_TECHNIQUE

	/* Technique keywords */
%token	TOKEN_RENDERER TOKEN_LANGUAGE TOKEN_INCLUDE TOKEN_PASS

	/* Pass keywords */
%token	TOKEN_VERTEX TOKEN_FRAGMENT TOKEN_GEOMETRY TOKEN_HULL TOKEN_DOMAIN TOKEN_COMPUTE

%token	TOKEN_FILLMODE TOKEN_CULLMODE TOKEN_DEPTHBIAS TOKEN_SDEPTHBIAS
%token	TOKEN_DEPTHCLIP TOKEN_SCISSOR TOKEN_MULTISAMPLE TOKEN_AALINE

%token	TOKEN_DEPTHREAD TOKEN_DEPTHWRITE TOKEN_COMPAREFUNC TOKEN_STENCIL
%token	TOKEN_STENCILREADMASK TOKEN_STENCILWRITEMASK TOKEN_STENCILOPFRONT TOKEN_STENCILOPBACK
%token	TOKEN_FAIL TOKEN_ZFAIL TOKEN_PASS

%token	TOKEN_ALPHATOCOVERAGE TOKEN_INDEPENDANTBLEND TOKEN_TARGET TOKEN_INDEX
%token	TOKEN_BLEND TOKEN_COLOR TOKEN_ALPHA TOKEN_WRITEMASK
%token	TOKEN_SOURCE TOKEN_DEST TOKEN_OP

	/* Sampler state keywords */
%token	TOKEN_ADDRMODE TOKEN_MINFILTER TOKEN_MAGFILTER TOKEN_MIPFILTER
%token	TOKEN_MAXANISO TOKEN_MIPBIAS TOKEN_MIPMIN TOKEN_MIPMAX
%token	TOKEN_BORDERCOLOR TOKEN_U TOKEN_V TOKEN_W

%type <nodePtr>		shader;
%type <nodeOption>	shader_statement;
%type <nodeOption>	shader_option;

%type <nodePtr>		technique;
%type <nodePtr>		technique_header;
%type <nodeOption>	technique_statement;
%type <nodeOption>	technique_option;

%%

shader
	: /* empty */				{ }
	| shader_statement shader	{ nodeOptionsAdd(parse_state->memContext, parse_state->topNode->options, &$1); }
	;

shader_statement
	: shader_option
	| technique			{ $$.type = OT_Technique; $$.value.nodePtr = $1; }
	;

shader_option
	: TOKEN_SEPARABLE '=' TOKEN_BOOLEAN ';'		{ $$.type = OT_Separable; $$.value.intValue = $3; }
	| TOKEN_QUEUE '=' TOKEN_INTEGER ';'			{ $$.type = OT_Queue; $$.value.intValue = $3; }
	| TOKEN_PRIORITY '=' TOKEN_INTEGER ';'		{ $$.type = OT_Priority; $$.value.intValue = $3; }
	;

technique
	: technique_header '{' technique_body '}' ';' { nodePop(parse_state); $$ = $1; }
	;

technique_header
	: TOKEN_TECHNIQUE '=' 
		{ 
			$$ = nodeCreate(parse_state->memContext, NT_Technique); 
			nodePush(parse_state, $$);
		}
	;

technique_body
	: /* empty */
	| technique_statement technique_body		{ nodeOptionsAdd(parse_state->memContext, parse_state->topNode->options, &$1); }
	;

technique_statement
	: technique_option
	;

technique_option
	: TOKEN_RENDERER '=' TOKEN_STRING ';'	{ $$.type = OT_Renderer; $$.value.strValue = $3; }
	| TOKEN_LANGUAGE '=' TOKEN_STRING ';'	{ $$.type = OT_Language; $$.value.strValue = $3; }
	| TOKEN_INCLUDE '=' TOKEN_STRING ';'	{ $$.type = OT_Include; $$.value.strValue = $3; }

%%