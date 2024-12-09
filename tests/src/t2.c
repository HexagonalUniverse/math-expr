/*	<tests/src/t1.c>

	An example test source-file for the application.
	Prints each argument in a separate, with the characters in string swap with their successor in ASCII. */

#include <stdio.h>
#include <stdlib.h>
#include <hex.h>


static inline void do_the_processing(const char * _String)
{
    for (size_t j = 0; _String[j] != '\0'; ++ j)
        putchar(_String[j] + 1);
    putchar('\n');
}


int main(int argc, char ** argsv)
{
    // argument input
    for (int i = 1; i < argc; ++ i)
        do_the_processing(argsv[i]);

    // stdin input
    char string_buffer[256] = { 0 };
    while (! feof(stdin))
    {
        fgets(string_buffer, 256, stdin);
        do_the_processing(string_buffer);
    }

    return 0;
}
