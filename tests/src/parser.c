/*	<tests/src/p1.c
	
*/

#include <hex.h>
#include <stdio.h>


int main(int argc, char ** argsv) {
	struct tokenc p_exp;
	double v;
	int parse_rc;

	for (int i = 1; i < argc; ++ i) {
		parse_rc = parse_expression(argsv[i], &p_exp);
		if (parse_rc != 0) {
			printf("S%d\n", parse_rc);
			continue;
		}

		evaluate_expression(&p_exp, NULL, &v);
		printf("%lf\n", v);
	}
	return 0;
}
