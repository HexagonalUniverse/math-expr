/*	<src/expressions.h>

	Defines the parser and evaluator for algebraic expressions. */

#ifndef _HS_EXPRESSIONS_HEADER_
#define _HS_EXPRESSIONS_HEADER_

#include <stdlib.h> // for "size_t"


	/*	Pre-definitions
		---------------

		Useful abreviations in this context. */

#define in_range(_A, _B, _X)	( ((_B) >= (_X)) && ((_X) >= (_A)) )
#define is_blank(_X)			( ((_X) == ' ' ) || ((_X) == '\n' ) )
#define is_operator(_X)			( ((_X) == '+') || ((_X) == '-') || ((_X) == '*') || ((_X) == '/') )
#define is_parenthesis(_X)		( ((_X) == '(') || ((_X) == ')') )
#define is_numeral(_X)			in_range('0', '9', (char) _X)
#define is_symbol(_X)			in_range('a', 'z', (char) _X)


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
// (Essentially Lexical Analysis)



/*	=======
	Parsing
	======= */

	/*	Inputs an expression (string) and returns (by ref.) its tokens in reverse-polish notation.*/
int parse_expression(const char *, struct tokenc * const);
// (Essentially Syntax Analysis)


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
// (There goes Semantic Analysis)



#endif // _HS_EXPRESSIONS_HEADER_
