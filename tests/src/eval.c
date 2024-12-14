/*	<tests/src/eval.c

	Tests for evaluation.
	Input a single string and prints outputs (stdout) the numerical value, with 6 decimal places. */

#include <hex.h>
#include <stdio.h>
#include <string.h>

static struct s_table SYMBOL_TABLE = { 0 };


static int __eval_test__(const char * _Expression) {
	const struct tokenc p_exp;
	if (parse_expression(_Expression, &p_exp) != 0)
		return 1;

	double v;
	if (evaluate_expression(&p_exp, &SYMBOL_TABLE, &v) != 0)
		return 2;

	printf("%.6lf\n", v);
	return 0;
}

int main(int argc, char ** argsv) {
	strcpy(SYMBOL_TABLE.symbols[0], "x");
	SYMBOL_TABLE.size ++; SYMBOL_TABLE.values[0] = 18;

	strcpy(SYMBOL_TABLE.symbols[1], "y");
	SYMBOL_TABLE.size ++; SYMBOL_TABLE.values[1] = 443;

	strcpy(SYMBOL_TABLE.symbols[2], "z");
	SYMBOL_TABLE.size ++; SYMBOL_TABLE.values[2] = 888;

	for (int i = 1; i < argc; ++ i) {
		if (__eval_test__(argsv[i]) != 0)
			return i;
	}
	return 0;
}
