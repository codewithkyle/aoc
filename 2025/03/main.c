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

uint64_t part_2(FILE *file)
{
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
    return joltage_sum;
}

int part_1(FILE *file)
{
    int joltage_sum = 0;
    char line[4096];
    while (fgets(line, sizeof(line), file))
    {
        int len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
        {
            line[len-1] = '\0';
            len--;
        }

        int left = 0;
        int right = 0;
        int left_idx = 0;
        for (int i = 0; i < len-1; i++)
        {
            int l_value = line[i] - 0x30;
            int r_value = line[i+1] - 0x30;
            if (l_value > left)
            {
                left = l_value;
                right = 0;
            }
            if (r_value > right)
            {
                right = r_value;
            }
        }

        joltage_sum += left * 10 + right;
    }
    return joltage_sum;
}

int main()
{
    FILE* file = fopen("data.txt", "r");
    if (!file)
    {
        perror("failed to open file");
        return -1;
    }
    int part_1_sum = part_1(file);
    rewind(file);
    uint64_t part_2_sum = part_2(file);
    fclose(file);

    printf("Part 1: %u\n", part_1_sum);
    printf("Part 2: %" PRIu64 "\n", part_2_sum);

    return 0;
}
