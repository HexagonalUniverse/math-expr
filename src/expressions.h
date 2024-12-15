/*	<src/expressions.h>

	Defines the parser and evaluator for algebraic expressions. */

#ifndef _HS_EXPRESSIONS_HEADER_
#define _HS_EXPRESSIONS_HEADER_

#include <stdlib.h> // for "size_t"


#if ! defined(__HSEXP_STDERR)
	#define __HSEXP_STDERR 0
#endif // ! defined(__HSEXP_STDERR)

/*	Pre-definitions
	---------------

	Useful abreviations in this context. */

#define in_range(_A, _B, _X)	( ((_B) >= (_X)) && ((_X) >= (_A)) )
#define is_blank(_X)			( ((_X) == ' ' ) || ((_X) == '\n' ) )
#define is_operator(_X)			( ((_X) == '+') || ((_X) == '-') || ((_X) == '*') || ((_X) == '/') || ((_X) == '^') || ((_X) == '%'))
#define is_parenthesis(_X)		( ((_X) == '(') || ((_X) == ')') )
#define is_numeral(_X)			in_range('0', '9', (char) _X)
#define is_symbol(_X)			( ((_X) == '_') || in_range('a', 'z', (char) _X) || in_range('A', 'Z', (char) _X) ) 


/*	==========
	Tokenizing
	========== */

#define TOKEN_SIZE	0x40u	// Token's max size.
#define N_TOKENS	0x400u	// Especifies the max number of tokens.

			/*	A token is a limited-size string. */
typedef char token_t[TOKEN_SIZE];

/*	Token (fixed size) container. */
struct tokenc {
	size_t qtt;
	token_t tokens[N_TOKENS];
};

/*	Tokenizes the given expression. */
struct tokenc tokenize_expression(const char *);
// (Essentially does Lexical Analysis)



/*	=======
	Parsing
	======= */

/*	Inputs an expression (string) and returns (by ref.) its tokens in reverse-polish notation.*/
enum PARSE_ERROR {
	PE_OK,
	PE_PARENTHESIS_CLOSE_MISMATCH, // more parenthesis closed than open.
	PE_PARENTHESIS_OPEN_MISMATCH, // more parenthesis open than closed..
	PE_CONSEC_OPERATORS,
	PE_CONSEC_OPERANDS,
	PE_INVALID_PARENTHESIS_OP_CONTEXT,
	PE_UNKNOWN,
};

enum PARSE_ERROR parse_expression(const char *, struct tokenc * const);
// (Essentially does Syntax Analysis)


/*	==========
	Evaluating
	========== */

#define ST_LENGTH	0x20u

struct s_table {
	size_t size;

	token_t symbols[ST_LENGTH];
	double values[ST_LENGTH];
};

int evaluate_expression(const struct tokenc *, const struct s_table *, double * const);
// (There goes the Semantic Analysis perhaps)


// (...)
bool symbol_token(const token_t);


#endif // _HS_EXPRESSIONS_HEADER_
