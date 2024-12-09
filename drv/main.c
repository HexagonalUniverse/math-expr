/*	<drv/main.c> 
	
	Sample driver source file associated with the project. */

#include <hex.h>
#include <stdio.h>

int main(void)
{
	printf("At main...\n");

	nothing(); // from <src/a.c>

	return 0;
}
