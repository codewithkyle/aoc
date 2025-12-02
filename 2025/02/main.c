#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>

unsigned long long identify_repeating(uint64_t start, uint64_t end)
{
    unsigned long long invalid_id_sum = 0;

    for (uint64_t i = start; i <= end; i++)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, i);
        size_t len = strlen(buffer);
        char pattern[32];
        pattern[0] = '\0';
        char out[32];
        for (uint j = 0; j < len && j < sizeof(pattern) - 1; j++)
        {
            strcpy(out, buffer + j);
            if (strcmp(pattern, out) == 0)
            {
                invalid_id_sum += i;
                break;
            }

            pattern[j] = buffer[j];
            pattern[j+1] = '\0';
        }
    }

    return invalid_id_sum;
}

int main()
{
    FILE *f = fopen("data.txt", "r");
    if (!f)
    {
        perror("fopen");
        return -1;
    }
    unsigned long long invalid_id_sum = 0;
    char line[4096];
    fgets(line, sizeof(line), f);
    char *token = strtok(line, ",");
    while (token != NULL)
    {
        char *range = token;
        char *dash = strchr(range, '-');
        if (dash)
        {
            *dash = '\0';
            char *left = range;
            char *right = dash + 1;
            uint64_t start = strtoull(left, NULL, 10);
            uint64_t end = strtoull(right, NULL, 10);
            invalid_id_sum += identify_repeating(start, end);
        }
        token = strtok(NULL, ",");
    }
    fclose(f);
    printf("%llu", invalid_id_sum);
    return 0;
}
