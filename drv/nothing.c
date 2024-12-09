/*	<drv/nothing.c>

	Sample driver source file for the project's data building
	post-processing. */

#include <hex.h>
#include <stdio.h>
#include <time.h>


int main(void)
{
	constexpr char filepath[] = "data/nothing.txt";
	
	FILE * the_file = fopen(filepath, "a");
	if (the_file == NULL)
		return 1;
	
	printf("Writing nothing.\n");
	fprintf(the_file, "Nothing. %lld\n", (long long) time(NULL));

	fclose(the_file);
	return 0;
}
