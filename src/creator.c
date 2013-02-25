#include <stdlib.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <inttypes.h>
#include "config.h"

#define LINE_SEED 1
#define LINE_OCTAVES 2
#define LINE_FREQUENCY 3
#define LINE_PERSISTENCE 4
#define LINE_WIDTH 5
#define LINE_SMOOTHING 18

void
trace (const char *s)
{
    fprintf (stderr, "==> %s\n", s);
}

void
usage (void)
{
    puts ("Usage: creator INFILE OUTFILE");
}

int
main (int argc, char **argv)
{
    if (argc != 3)
    {
        usage ();
        return EXIT_FAILURE;
    }


    FILE *file = NULL;
    file = fopen (argv[1], "rb");
    if (file == NULL)
    {
        trace ("Could not open file:");
        trace (argv[1]);
        return EXIT_FAILURE;
    }

    fseek (file, 0, SEEK_END);
    long file_size = ftell (file);
    fseek (file, 0, SEEK_SET);

    char *file_buf = malloc (file_size + 1);
    if (file_buf == NULL)
    {
        perror (argv[1]);
        return EXIT_FAILURE;
    }

    fread (file_buf, 1, file_size, file);
    file_buf[file_size + 1] = '\0';
    fclose (file);

    FILE *outfile = fopen (argv[2], "wb");
    if (outfile == NULL)
    {
        trace ("Could not open file:");
        trace (argv[1]);
        return EXIT_FAILURE;
    }

    char *str1, *str2, *token, *subtoken;
    char *saveptr1, *saveptr2;
    int j;
    int line = 0;

    for (j = 1, str1 = file_buf;; j++, str1 = NULL)
    {
        token = strtok_r (str1, "\n", &saveptr1);
        if (token == NULL)
            break;

        line++;
        str2 = token;
        subtoken = strtok_r (str2, " ", &saveptr2);
        if (subtoken != NULL)
        {

            switch (line)
            {

                case LINE_PERSISTENCE:
                    {
                        double buf = strtod (subtoken, NULL);
                        if (errno == ERANGE)
                        {
                            perror ("Could not convert string to double.");
                            continue;
                        }
                        printf ("[%s|%f]\n", subtoken, buf);
                        fwrite (&buf, sizeof (double), 1, outfile);
                        break;
                    }

                case LINE_SEED:
                case LINE_WIDTH:
                    {
                        uint32_t buf = strtol (subtoken, NULL, 0);
                        if (errno == ERANGE)
                        {
                            perror ("Could not convert string to integer.");
                            continue;
                        }
                        printf ("[%s|%" PRIu32 "]\n", subtoken, buf);
                        fwrite (&buf, sizeof (uint32_t), 1, outfile);
                        break;
                    }

                case LINE_OCTAVES:
                case LINE_FREQUENCY:
                case LINE_SMOOTHING:
                    {
                        uint16_t buf = strtol (subtoken, NULL, 0);
                        if (errno == ERANGE)
                        {
                            perror ("Could not convert string to integer.");
                            continue;
                        }
                        printf ("[%s|%" PRIu16 "]\n", subtoken, buf);
                        fwrite (&buf, sizeof (uint16_t), 1, outfile);
                        break;
                    }

                default:
                    {
                        uint8_t buf = strtol (subtoken, NULL, 0);
                        if (errno == ERANGE)
                        {
                            perror ("Could not convert string to integer.");
                            continue;
                        }
                        printf ("[%s|%" PRIu8 "]\n", subtoken, buf);
                        fwrite (&buf, sizeof (uint8_t), 1, outfile);
                        break;
                    }

            }
        }

    }

    free (file_buf);
    fclose (outfile);
    return 0;
}
