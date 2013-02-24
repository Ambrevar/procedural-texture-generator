#include <stdlib.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include "config.h"

#define SUBSTR_MAX 1024

void
trace (const char *s)
{
    fprintf (stderr, "==> %s\n", s);
}

void usage(void)
{
    puts(
        "Usage: creator INFILE OUTFILE"
        );
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        usage();
        return EXIT_FAILURE;
    }


    FILE *file = NULL;
    file = fopen (argv[1], "rb");
    if (file == NULL)
    {
        trace("Could not open file:");
        trace(argv[1]);
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char * file_buf = malloc (file_size+1);
    if (file_buf == NULL)
    {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    fread( file_buf, 1, file_size, file);
    file_buf[file_size+1] = '\0';
    fclose(file);

    FILE * outfile=fopen(argv[2], "wb");
    if (outfile == NULL)
    {
        trace("Could not open file:");
        trace(argv[1]);
        return EXIT_FAILURE;        
    }

    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int j;

    for (j = 1, str1 = file_buf; ; j++, str1 = NULL) {
        token = strtok_r(str1, "\n", &saveptr1);
        if (token == NULL)
            break;

        printf("|%s\n", token);

        subtoken = strtok_r(str2, " ", &saveptr2);
        if (subtoken == NULL)
            printf(" --> %s\n", subtoken);
        /* for (str2 = token; ; str2 = NULL) { */
        /*     subtoken = strtok_r(str2, " ", &saveptr2); */
        /*     if (subtoken == NULL) */
        /*         break; */
        /*     printf(" --> %s\n", subtoken); */
        /* } */
    }

    free(file_buf);
    fclose(outfile);
    return 0;
}
