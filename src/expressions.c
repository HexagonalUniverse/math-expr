/*	<src/expressions.c>

	Defines the parser and evaluator for algebraic expressions.  */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "expressions.h"


struct tokenc tokenize_expression(const char * _Expression) {
	const size_t length = strlen(_Expression);

	token_t token_buffer = { 0 };
	size_t buffer_size = 0u;

	struct tokenc yield = { 0 };

	for (size_t i = 0; i < length; ++ i) {
		const char char_buffer = _Expression[i];

		if (is_blank(char_buffer)) {
			/* End of token. */

			if (buffer_size > 0u) {
				token_buffer[buffer_size] = '\0';
				strcpy(yield.tokens[yield.qtt ++], token_buffer);
				buffer_size = 0u;
			}
			continue;
		}
		else if (is_parenthesis(char_buffer) || is_operator(char_buffer)) {
			/* Parenthesis or operand. */

			if (buffer_size > 0u) {
				token_buffer[buffer_size] = '\0';
				strcpy(yield.tokens[yield.qtt ++], token_buffer);
				buffer_size = 0u;
			}

			token_buffer[0] = char_buffer;
			token_buffer[1] = '\0';

			strcpy(yield.tokens[yield.qtt ++], token_buffer);
			buffer_size = 0u;
		}
		else {
			/* Integrating token. */
			token_buffer[buffer_size ++] = char_buffer;
		}
	}

	if (buffer_size > 0u) {
		token_buffer[buffer_size] = '\0';
		strcpy(yield.tokens[yield.qtt ++], token_buffer);
		buffer_size = 0u;
	}

	return yield;
}


/*	The parsing happens in reverse polish notation for easier evaluation. */


/*	Tells whether the given token is a numeral. */
static inline bool
is_token_numeral(const token_t _Token) {
	size_t iterator = 0u;
	while (_Token[iterator] != '\0') {
		char buffer = _Token[iterator ++];
		if (is_numeral(buffer))
			continue;
		return false;
	}
	return true;
}

/*	Tells whether the given token is a symbol.
	It is determined by every character is verified to be a symbol.
	Attent to what some operators are considered symbols, therefore. */
static inline bool
is_token_symbol(const token_t _Token) {
	size_t iterator = 0u;
	while (_Token[iterator] != '\0') {
		char buffer = _Token[iterator ++];
		if (is_symbol(buffer))
			continue;
		return false;
	}
	return true;
}

/*	Tells whether the given token is an operator. */
static inline bool
is_token_operator(const token_t _Token) {
	if (_Token[1] != '\0')
	{
		if (strcmp(_Token, "sqrt") == 0)			return true;
		else if (strcmp(_Token, "log") == 0)		return true;
		else if (strcmp(_Token, "ln") == 0)			return true;
		else if (strcmp(_Token, "l2") == 0)			return true;
		else if (strcmp(_Token, "abs") == 0)		return true;
		else if (strcmp(_Token, "int") == 0)		return true;
		return false;
	}
	return is_operator(* _Token);
}

/*	Returns the precedence of a given operator.
	The smaller the number the bigger the precedence.
	Returns negative in case of an invalid operator. */
static inline int
operator_precedence(const token_t _Operator)
{
	/*	Verifying whether the operator have one character or not. */
	if (_Operator[1] != '\0') {
		if (strcmp(_Operator, "sqrt") == 0)
			return 4;

		else if (strcmp(_Operator, "log") == 0)
			return 4;
		
		else if (strcmp(_Operator, "ln") == 0)
			return 4;

		else if (strcmp(_Operator, "l2") == 0)
			return 4;

		else if (strcmp(_Operator, "int") == 0)
			return 4;

		else if (strcmp(_Operator, "abs") == 0)
			return 4;

		return 10;
	}

	switch (_Operator[0]) {
	case '+': // addition
	case '-': // subtraction
		return 6;

	case '*': // multiplication
	case '/': // division
	case '%': // modulus
		return 5;

	case '^': // exponentiation
		return 4;

	case '|': // absolute (NYI)
		return 3;

	case '(': // scoping
	case ')':
		return 2;
	}

	return -2;
}

/*	Returns the number of operators of a given operator. */
static inline int
operator_operands(const token_t _Operator)
{
	if (_Operator[1] == '\0') {
		switch (_Operator[0]) {
		case '+':
		case '-':
		case '*':
		case '/': 
		case '^':
		case '%':
			return 2;

		case '(': // * TO EXPLAIN
			return 1;

		default:
			return -1;
		}
	}

	// Unaries (single var. functions)
	return 1;
#if 0 // TODO: add others...
	if (strcmp(_Operator, "abs") == 0)
		return 1;
	else if (strcmp(_Operator, "log") == 0)
		return 1;
	else if (strcmp(_Operator, "sqrt") == 0)
		return 1;
	else if (strcmp(_Operator, "int") == 0)
		return 1;
	return -2;
#endif
}

/*	Defines what is the maximum of enclosure depth (parenthesis) an expression can have. */
#define EXP_MAX_DEPTH	0X20u

enum PARSE_ERROR {
	PE_OK,
	PE_PARENTHESIS_CLOSE_MISMATCH, // more parenthesis closed than open.
	PE_PARENTHESIS_OPEN_MISMATCH, // more parenthesis open than closed..
	PE_CONSEC_OPERATORS,
	PE_CONSEC_OPERANDS,
	PE_INVALID_PARENTHESIS_OP_CONTEXT,
};

int parse_expression(const char * expression, struct tokenc * const parsed_symbols)
{
	// Tokenized expression.
	const struct tokenc tok_exp = tokenize_expression(expression);

	// The parsed result buffer.	
	* parsed_symbols = (struct tokenc) { 0 };

	token_t operator_stack[N_TOKENS] = { 0 };
	size_t stack_size = 0u;

	int last_type = 0; // tracks the last token type.
	
	int parenthesis_count = 0; // how "deep" enclosed are we
	int parenthesis_context[EXP_MAX_DEPTH] = { 0 }; // parenthesis stack. Counts how many operands are there in one context.
	
	// tracks whether a "term" (often operands) was enclosed.
	bool enclosed = false;

	// iterating over the tokens.
	for (size_t i = 0; i < tok_exp.qtt; ++ i) {
		
		// Adding a operator onto the stack.
		if (is_token_operator(tok_exp.tokens[i])) {
			const int op_operands = operator_operands(tok_exp.tokens[i]);

			// operator following operator is only allowed for unaries.
			if ((last_type == 1) && (op_operands != 1)) {
				fprintf(stderr, "Operator following operator!\n");
				return PE_CONSEC_OPERATORS;
			}

			// recognizes a new term given by the unary operator.
			if ((! enclosed) && (op_operands == 1)) {
				enclosed = true;
				++ parenthesis_context[parenthesis_count];
				// fprintf(stderr, "[+] (UNARY) P[%d] = %d\n", parenthesis_count, parenthesis_context[parenthesis_count]);
			}

			// preparing for the start of an operand.
			else if (enclosed && (op_operands == 2)) {
				enclosed = false;
			}


			last_type = 1;

			const int precedence = operator_precedence(tok_exp.tokens[i]);
			while ((stack_size > 0u) && (operator_stack[stack_size - 1][0] != '(') && (operator_precedence(operator_stack[stack_size - 1]) < precedence)) {
				strcpy(parsed_symbols->tokens[parsed_symbols->qtt ++], operator_stack[-- stack_size]);
				
				// subtracting one
				parenthesis_context[parenthesis_count] -= (operator_operands(operator_stack[stack_size]) - 1);
			}

			strcpy(operator_stack[stack_size ++], tok_exp.tokens[i]);
		}

		// Adding a number / symbol onto the queue.
		else if (is_token_numeral(tok_exp.tokens[i]) || is_token_symbol(tok_exp.tokens[i])) {
			if (last_type == 2) {
				// fprintf(stderr, "Operand following operand!\n");
				return PE_CONSEC_OPERANDS;
			}

			// if (! ((last_type == 1) && (operator_operands(tok_exp.tokens[i - 1]) == 1)))
			// increments the # of terms of the enclosure as long the current one isn't closed.
			if (! enclosed) {
				++ parenthesis_context[parenthesis_count];
				// fprintf(stderr, "[+] (numeral) P[%d] = %d\n", parenthesis_count, parenthesis_context[parenthesis_count]);
			}

			last_type = 2;

			strcpy(parsed_symbols->tokens[parsed_symbols->qtt ++], tok_exp.tokens[i]);
		}

		// Left parenthesis.
		else if (strcmp(tok_exp.tokens[i], "(") == 0) {

			strcpy(operator_stack[stack_size ++], tok_exp.tokens[i]);

			// last token was a binary operator.
			/*
			if ((last_type == 1) && (operator_operands(tok_exp.tokens[i - 1]) == 2)) {
				++ parenthesis_context[parenthesis_count];
				fprintf(stderr, "[+] (left p) P[%d] = %d\n", parenthesis_count, parenthesis_context[parenthesis_count]);
			}
			*/
			if (! enclosed) {
				++ parenthesis_context[parenthesis_count];
				// fprintf(stderr, "[+] (left p) P[%d] = %d\n", parenthesis_count, parenthesis_context[parenthesis_count]);
			}

			last_type = 3;
			++ parenthesis_count;
			enclosed = false;

		}

		// Right parenthesis.
		else if (strcmp(tok_exp.tokens[i], ")") == 0) {
			last_type = 4;

			if (parenthesis_count < 0) {
				fprintf(stderr, "Mismatched parenthesis (CLOSE)!\n");
				return PE_PARENTHESIS_CLOSE_MISMATCH;
			}

			while ((stack_size > 0u) && (strcmp(operator_stack[stack_size - 1], "(") != 0)) {
				strcpy(parsed_symbols->tokens[parsed_symbols->qtt ++], operator_stack[-- stack_size]);

				// printf("OP(\"%s\"): %d\n", operator_stack[stack_size], operator_operands(operator_stack[stack_size]));
				parenthesis_context[parenthesis_count] -= (operator_operands(operator_stack[stack_size]) - 1);
				// fprintf(stderr, "pc: "); for (int k = 0; k <= parenthesis_count; ++k) fprintf(stderr, "%d ", parenthesis_context[k]); fprintf(stderr, "\n");
			}

			if (parenthesis_context[parenthesis_count] != 1) {
				fprintf(stderr, "parenthesis #%d is broken: %d. Last token: \"%s\" (%llu)\n", parenthesis_count, parenthesis_context[parenthesis_count], tok_exp.tokens[i], i);
				return PE_INVALID_PARENTHESIS_OP_CONTEXT;
			}

			parenthesis_context[parenthesis_count] = 0;
			-- parenthesis_count;
			stack_size --; // removing the "("
		}


	#if 0
		fprintf(stderr, "\n-> \"%s\"\n", tok_exp.tokens[i]);
		//for (size_t j = i; j < tok_exp.qtt; ++ j) fprintf(stderr, "%s ", tok_exp.tokens[j]);

		fprintf(stderr, "\tStack(%llu): [ ", stack_size);
		for (size_t j = 0; j < stack_size; ++ j) {
			fprintf(stderr, "%s ", operator_stack[j]);
		}
		fprintf(stderr, "]\n\tQueue(%llu): [ ", parsed_symbols->qtt);
		for (size_t j = 0; j < parsed_symbols->qtt; ++ j)
			fprintf(stderr, "%s ", parsed_symbols->tokens[j]);
		fprintf(stderr, "]\n");
		fprintf(stderr, "\tPC(%d): ", parenthesis_count); for (int k = 0; k <= parenthesis_count; ++k) fprintf(stderr, "%d ", parenthesis_context[k]);
		fprintf(stderr, "\n\n");
	#endif

	}

	if (parenthesis_count != 0) {
		fprintf(stderr, "Mismatched parenthesis (2)!\n");
		return PE_PARENTHESIS_OPEN_MISMATCH;
	}

	while (stack_size > 0u) {
		strcpy(parsed_symbols->tokens[parsed_symbols->qtt ++], operator_stack[-- stack_size]);
		
		parenthesis_context[parenthesis_count] -= (operator_operands(operator_stack[stack_size]) - 1);
	}
	
	// fprintf(stderr, "pc: "); for (int k = 0; k <= parenthesis_count; ++k) fprintf(stderr, "%d ", parenthesis_context[k]); fprintf(stderr, "\n\n");

	if (parenthesis_context[parenthesis_count] != 1) {
		fprintf(stderr, "unmatch operands\n");
		return PE_INVALID_PARENTHESIS_OP_CONTEXT ;
	}

#if 0
	printf("Parsed expression: ");
	for (size_t i = 0; i < parsed_symbols->qtt; ++ i)
		printf("%s ", parsed_symbols->tokens[i]);
	printf("\n");
#endif

	return PE_OK;
}

static inline double
evaluate_binary_operation(double _Operand1, double _Operand2, const token_t _Operator)
{
	if (_Operator[1] != '\0') {
		fprintf(stderr, "[%s] Not yet implemented1\n", __func__);
		return -1;
	}

	switch (_Operator[0]) {
	case '+':
		return _Operand1 + _Operand2;
	case '-':
		return _Operand1 - _Operand2;
	case '*':
		return _Operand1 * _Operand2;
	case '/':
		return _Operand1 / _Operand2;
	case '^':
		return pow(_Operand1, _Operand2);
	case '%':
		return _Operand1 - floor(_Operand1 / _Operand2) * _Operand2;
	}

	fprintf(stderr, "[%s] Not yet implemented 2: %s\n", __func__, _Operator);
	return -2;
}

static inline double
__absolute(double _X) {
	if (_X < 0.0)
		return - _X;
	return _X;
}

static inline double
evaluate_unary_operation(double _Operand, const token_t _Operator)
{
	if (strcmp(_Operator, "sqrt") == 0) {
		return sqrt(_Operand);
	}
	else if (strcmp(_Operator, "ln") == 0) {
		return log(_Operand);
	}
	else if (strcmp(_Operator, "log") == 0) {
		return log10(_Operand);
	}
	else if (strcmp(_Operator, "l2") == 0) {
		return log2(_Operand);
	}

	else if (strcmp(_Operator, "abs") == 0) {
		return __absolute(_Operand);
	}
	else if (strcmp(_Operator, "int") == 0) {
		return 0;
	}

	fprintf(stderr, "[%s] error\n", __func__);
	return -1;
}

int evaluate_expression(const struct tokenc * rp_tokens, const struct s_table * symbol_table, double * const _ReturnValue)
{
	double evaluation_stack[N_TOKENS] = { 0 };
	size_t stack_size = 0u;

	const token_t * rp_exp = rp_tokens->tokens;

	for (size_t i = 0; i < rp_tokens->qtt; ++ i) {

		if (is_token_numeral(rp_exp[i])) {
			const double numerical_value = atof(rp_exp[i]);

			evaluation_stack[stack_size ++] = numerical_value;
			continue;
		}
		else if (is_token_operator(rp_exp[i])) {
			const int n_operands = operator_operands(rp_exp[i]);

			if ((n_operands < 0) || (n_operands > 2)) {
				fprintf(stderr, "ERROR\n");
				continue;
			}

			// Unary operators
			else if (n_operands == 1) {
				const double operand = evaluation_stack[-- stack_size];

				evaluation_stack[stack_size ++] = evaluate_unary_operation(operand, rp_exp[i]);
			}

			// Binary operators
			else if (n_operands == 2) {
				const double operand_2 = evaluation_stack[-- stack_size];
				const double operand_1 = evaluation_stack[-- stack_size];

				evaluation_stack[stack_size ++] = evaluate_binary_operation(operand_1, operand_2, rp_exp[i]);
			}
		}
		else if (is_token_symbol(rp_exp[i])) {
			double symbol_value = 0.0;

			if (symbol_table != NULL)
			{
				// seeking symbol
				size_t j = 0;
				for (; j < symbol_table->size; ++ j) {
					if (strcmp(symbol_table->symbols[j], rp_exp[i]) == 0)
						break;
				}

				if (j < symbol_table->size)
					symbol_value = symbol_table->values[j];
			}

			evaluation_stack[stack_size ++] = symbol_value;
			continue;
		}
	}

	* _ReturnValue = evaluation_stack[0];
	return 0;
}


