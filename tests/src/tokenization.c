/*	<tests/src/tokenization.c

	Tests for tokenization. 
	Input a single string and prints in multiple lines its tokens. */

#include <hex.h>
#include <stdio.h>


static void __tokenize__(const char * _Expression) {
	const struct tokenc tok_exp = tokenize_expression(_Expression);

	for (size_t i = 0; i < tok_exp.qtt; ++ i)
		printf("%s\n", tok_exp.tokens[i]);
}


int main(int argc, char ** argsv) {
	if (argc > 2)
		return 1;
	__tokenize__(argsv[1]);
	return 0;
}
