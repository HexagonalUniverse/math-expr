/*	<tests/src/t1.c>
	
	An example test source-file for the application.
	Sums the numbers passed as arguments, and prints the output on stdout. */

#include <stdio.h>
#include <stdlib.h>
#include <hex.h>


static inline int is_numeral(const char * _String)
{
    int char_buffer;
    for (size_t i = 0; _String[i] != '\0'; ++ i)
    {
        char_buffer = (int) _String[i];
        if (char_buffer < 48 || char_buffer > 57)
            return 0;
    }
    return 1;
}

int main(int argc, char ** argsv)
{
	int buffer = 0;

	for (int i = 1; i < argc; ++ i)
	{
	    if (! is_numeral(argsv[i])) {
	        return i;
	    }

		buffer += atoi(argsv[i]);
	}

	printf("%d\n", buffer);
	return 0;
}
