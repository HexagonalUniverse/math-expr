

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define TOKEN_SIZE	0x40u	// Token's max size.
#define N_TOKENS	0x400u

typedef char token_t[TOKEN_SIZE];

static token_t tokens[N_TOKENS] = { 0 };
static size_t n_tokens = 0u;


#define in_range(_A, _B, _X)	( ((_B) >= (_X)) && ((_X) >= (_A)) )
#define is_blank(_X)			( ((_X) == ' ' ) || ((_X) == '\n' ) )
#define is_operator(_X)			( ((_X) == '+') || ((_X) == '-') || ((_X) == '*') || ((_X) == '/') )
#define is_parenthesis(_X)		( ((_X) == '(') || ((_X) == ')') )
#define is_numeral(_X)			in_range('0', '9', _X)
#define is_symbol(_X)			in_range('a', 'z', _X)


static void __tokenize(const char * _Expression)
{
	const size_t length = strlen(_Expression);

	token_t token_buffer = { 0 };
	size_t buffer_size = 0u;

	for (size_t i = 0; i < length; ++ i) {
		const char char_buffer = _Expression[i];

		if (is_blank(char_buffer)) {
			/* End of token. */

			if (buffer_size > 0u) {
				token_buffer[buffer_size] = '\0';
				strcpy(tokens[n_tokens ++], token_buffer);
				buffer_size = 0u;
			}
			continue;
		}
		else if (is_parenthesis(char_buffer) || is_operator(char_buffer)) {
			/* Parenthesis or operand. */

			if (buffer_size > 0u) {
				token_buffer[buffer_size] = '\0';
				strcpy(tokens[n_tokens ++], token_buffer);
				buffer_size = 0u;
			}

			token_buffer[0] = char_buffer;
			token_buffer[1] = '\0';

			strcpy(tokens[n_tokens ++], token_buffer);
			buffer_size = 0u;
		}
		else {
			/* Integrating token. */
			token_buffer[buffer_size ++] = char_buffer;
		}
	}

	if (buffer_size > 0u) {
		token_buffer[buffer_size] = '\0';
		strcpy(tokens[n_tokens ++], token_buffer);
		buffer_size = 0u;
	}
}


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


static inline bool
is_token_operator(const token_t _Token) {
	if (_Token[1] != '\0')
	{
		if (strcmp(_Token, "sqrt") == 0) return true;
		if (strcmp(_Token, "abs") == 0) return true;
		if (strcmp(_Token, "int") == 0) return true;
		return false;
	}
	return is_operator(* _Token);
}

static inline int
operator_precedence(const token_t _Operator)
{
	/*	Verifying whether the operator have one character or not. */
	if (_Operator[1] != '\0') {
		if (strcmp(_Operator, "sqrt") == 0)
			return 2;

		else if (strcmp(_Operator, "int") == 0) {
			return 6;
		}

		return 9;
	}

	switch (_Operator[0]) {
	case '+': // addition
	case '-': // subtraction
		return 5;

	case '*': // multiplication
	case '/': // division
	case '%': // modulus
		return 4;

	case '^': // exponentiation
		return 3;

	case '|': // absolute
		return 2;

	case '(': // scoping
	case ')':
		return 1;
	}

	return 9;
}

static token_t output_queue[N_TOKENS] = { 0 };
static size_t queue_size = 0u;

static void __parse_expression(const char * _Expression)
{
	token_t operator_stack[N_TOKENS] = { 0 };
	size_t stack_size = 0u;

	for (size_t i = 0; i < n_tokens; ++ i) {
		printf("Token: %s\nTokens left: ", tokens[i]);
		for (size_t j = i; j < n_tokens; ++ j) {
			printf("%s ", tokens[j]);
		}

		printf("\nPilha: ");
		for (size_t j = 0; j < stack_size; ++ j) {
			printf("%s ", operator_stack[j]);
		}
		printf("\nFila: ");
		for (size_t j = 0; j < queue_size; ++ j) {
			printf("%s ", output_queue[j]);
		}
		printf("\n\n");

		// Adding a operator onto the stack.
		if (is_token_operator(tokens[i])) {
			const int precedence = operator_precedence(tokens[i]);
			printf("[Operator = %c], prec = %d\n", tokens[i][0], precedence);

			strcpy(operator_stack[stack_size ++], tokens[i]);
			while ((stack_size > 0u) && (operator_precedence(operator_stack[stack_size - 1]) < precedence)) {
				printf("Removendo \"%s\" da pilha\n", operator_stack[stack_size - 1]);

				strcpy(output_queue[queue_size ++], operator_stack[-- stack_size]);
			}

		}

		// Adding a number / symbol onto the queue.
		else if (is_token_numeral(tokens[i]) || is_token_symbol(tokens[i])) {
			printf("+ %s [Number]\n", tokens[i]);
			strcpy(output_queue[queue_size ++], tokens[i]);
		}

		// Left parenthesis.
		else if (strcmp(tokens[i], "(") == 0) {
			strcpy(operator_stack[stack_size ++], tokens[i]);
		}

		// Right parenthesis.
		else if (strcmp(tokens[i], ")") == 0) {
			while ((stack_size > 0u) && (strcmp(operator_stack[stack_size - 1], "(") != 0)) {
				strcpy(output_queue[queue_size ++], operator_stack[-- stack_size]);
			}

			stack_size --;
		}
	}

	while (stack_size > 0u) {
		strcpy(output_queue[queue_size ++], operator_stack[-- stack_size]);
	}

	printf("Fila de saída:\n");
	for (size_t i = 0; i < queue_size; ++ i) {
		printf("> %s\n", output_queue[i]);
	}
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
	}

	fprintf(stderr, "[%s] Not yet implemented 2: %s\n", __func__, _Operator);
	return -2;
}


static inline double
__evaluate(size_t _TokenQtt, const token_t _RPTokens[static _TokenQtt])
{
	double evaluation_stack[N_TOKENS] = { 0 };
	size_t stack_size = 0u;

	for (size_t i = 0; i < _TokenQtt; ++ i) {
		printf("Stack: ");
		for (size_t j = 0; j < stack_size; ++ j) {
			printf("%lf ", evaluation_stack[j]);
		}
		printf("\n");

		if (is_token_numeral(_RPTokens[i])) {
			const double numerical_value = atof(_RPTokens[i]);

			evaluation_stack[stack_size ++] = numerical_value;
			continue;
		}
		else if (is_token_symbol(_RPTokens[i])) {
			evaluation_stack[stack_size ++] = 18.0;
			continue;
		}


		// If it is not a literal expression, it must be an operator, at this step.

		// Binary operators.
		const double operand_2 = evaluation_stack[-- stack_size];
		const double operand_1 = evaluation_stack[-- stack_size];

		evaluation_stack[stack_size ++] = evaluate_binary_operation(operand_1, operand_2, _RPTokens[i]);
	}

	return evaluation_stack[0];
}



/*
Shunting Yard Algorithm. Brilliant.org. Retrieved 20:30, December 4, 2024, from https://brilliant.org/wiki/shunting-yard-algorithm/
1.  While there are tokens to be read:
2.        Read a token
3.        If it's a number add it to queue
4.        If it's an operator
5.               While there's an operator on the top of the stack with greater precedence:
6.                       Pop operators from the stack onto the output queue
7.               Push the current operator onto the stack
8.        If it's a left bracket push it onto the stack
9.        If it's a right bracket
10.            While there's not a left bracket at the top of the stack:
11.                     Pop operators from the stack onto the output queue.
12.             Pop the left bracket from the stack and discard it
13. While there are operators on the stack, pop them to the queue
*/




int main(void) {

	const char expression[] = "4+s/(9-3)";

	__tokenize(expression);
	printf("Tokens: ");
	for (size_t i = 0, l = n_tokens - 1; i < l; ++ i)
		printf("<%lu: %s>, ", i, tokens[i]);
	printf("<%lu: %s>.\n", n_tokens - 1, tokens[n_tokens - 1]);

	__parse_expression(expression);
	printf("Result: %lf\n", __evaluate(queue_size, output_queue));
	return 0;
}
