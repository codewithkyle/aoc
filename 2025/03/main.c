#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

        char values[13];
        memset(values, '0', sizeof(values)-1);
        values[12] = '\0';

        size_t left_idx = 0;
        size_t offset = 11;
        for (size_t i = 0; i < len - offset; i++)
        {
            int value = line[i] - 0x30;
            if (value > values[0] - 0x30)
            {
                values[0] = line[i];
                left_idx = i;
            }
        }

        for (size_t idx = 1; idx < 12 && offset > 0; idx++)
        {
            offset--;
            for (size_t i = left_idx+1; i < len - offset; i++)
            {
                int value = line[i] - 0x30;
                if (value > values[idx] - 0x30)
                {
                    values[idx] = line[i];
                    left_idx = i;
                }
            }
        }

        joltage_sum += strtoull(values, NULL, 10);
    }
    printf("%" PRIu64 "\n", joltage_sum);

    return 0;
}
