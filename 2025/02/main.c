#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <inttypes.h>

uint repeated_substring(const char *buffer)
{
    size_t n = strlen(buffer);

    for (size_t len = 1; len <= n / 2; len++)
    {
        if (n % len != 0) continue;

        uint repeating = 1;
        char pattern[32];
        memcpy(pattern, buffer, len);
        pattern[len] = '\0';
        for (size_t pos = 0; pos < n; pos += len)
        {
            if (strncmp(buffer + pos, pattern, len) != 0)
            {
                repeating = 0;
                break;
            }
        }
        if (repeating) return 1;
    }
    return 0;
}

unsigned long long identify_repeating(uint64_t start, uint64_t end)
{
    unsigned long long invalid_id_sum = 0;

    for (uint64_t i = start; i <= end; i++)
    {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%" PRIu64, i);

        if (repeated_substring(buffer))
        {
            invalid_id_sum += i;
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
