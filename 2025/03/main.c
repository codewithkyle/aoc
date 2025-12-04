#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

enum { DIGITS = 12 };

// NOTE: rewrote after learning about monotonic stacks
void monotonic_stack(const char *line, size_t len, size_t K, char *out)
{
    size_t max_drops = len - K; // how many digits we can skip/drop
    char stack[4096];
    size_t top = 0; // stack size

    for (size_t i = 0; i < len; i++)
    {
        // for each digit we decide:
        // should it replace some weaker digit we already picked
        // or should it be appended
        // or should it be thrown away
        char d = line[i];

        // if the current digit is bigger than the last chosen digit
        // and we can still drop digits then drop the weaker one
        while (
            top > 0 && // the stack isn't empty
            d > stack[top-1] && // the digit is stronger
            max_drops > 0 // we can still throw away digits
        )
        {
            top--;
            max_drops--;
        }

        if (top < K) // append if we still need digits
        {
            stack[top++] = d;
        }
        else
        {
            max_drops--; // full up, drop it
        }
    }

    for (size_t i = 0; i < K; i++)
    {
        out[i] = stack[i];
    }
    out[K] = '\0';
}

int main()
{
    FILE* file = fopen("data.txt", "r");
    if (!file)
    {
        perror("failed to open file");
        return -1;
    }

    uint64_t joltage_sum = 0;
    char line[4096];
    while (fgets(line, sizeof(line), file))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
        {
            line[len-1] = '\0';
            len--;
        }

        char out[DIGITS+1];
        monotonic_stack(line, len, DIGITS, out);
        joltage_sum += strtoull(out, NULL, 10);
    }
    printf("%" PRIu64 "\n", joltage_sum);
    fclose(file);

    return 0;
}
