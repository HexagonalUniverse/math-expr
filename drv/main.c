/*	<drv/main.c> 
	
	(...) */

#include <hex.h>
#include <string.h>
#include <stdio.h>


static inline void __print_tokens(const struct tokenc * __tokenc) {
	for (size_t i = 0; i < __tokenc->qtt; ++ i)
		printf("%s ", __tokenc->tokens[i]);
}

int main(void)
{
	struct tokenc pexp;
	struct s_table S = { 0 };
	int rc;

	strcpy(S.symbols[0], "s");
	S.size ++;
	S.values[0] = 18;

	printf("=======================\n");
	rc = parse_expression("( abs (0 - 1) )", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

	printf("=======================\n");
	rc = parse_expression("sqrt 2", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

	printf("=======================\n");
	rc = parse_expression("sqrt sqrt 2", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

	printf("=======================\n");
	rc = parse_expression("1 + sqrt sqrt 16 + (20 / 5) - 2", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

#if 0

	rc = parse_expression("( abs (0 - 1) )", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

	rc = parse_expression("1 / sqrt 1", &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");

	rc = parse_expression("1 / sqrt 1 /",  &pexp);
	if (rc != 0) return 0;
	printf("rc: %d\n", rc);
	printf("Parsed: "); __print_tokens(&pexp); printf("\n\n");
#endif

	// printf("rc: %d\n", parse_expression("1 / (1 + (sqrt 1 /))", &pexp));
	return 0;
}
