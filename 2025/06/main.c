#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

uint64_t part_1(int *grid, char *ops, size_t rows, size_t cols)
{
    uint64_t sum = 0;
    for (size_t x = 0; x < cols; x++)
    {
        uint64_t col_total = 0;
        for (size_t y = 0; y < rows; y++)
        {
            int value = grid[y * cols + x];
            if (col_total == 0)
            {
                col_total = value;
            }
            else if (ops[x] == '*')
            {
                col_total *= value;
            }
            else if (ops[x] == '+')
            {
                col_total += value;
            }
        }
        sum += col_total;
    }
    return sum;
}

int main()
{
    FILE *f = fopen("data.txt", "r");
    if (!f)
    {
        perror("failed to open file");
        return -1;
    }

    char line[4096];
    size_t cols = 1;
    size_t rows = 0;
    uint8_t in_space = 0;
    while (fgets(line, sizeof(line), f))
    {
        rows++;
        if (cols == 1)
        {
            size_t len = strlen(line);
            line[len-1] = '\0';
            len--;

            for (size_t i = 0; i < len; i++)
            {
                int value = line[i] - 0x30;
                if (value >= 0 && value <= 9)
                {
                    in_space = 0;
                    continue;
                }
                else if (in_space) continue;
                else
                {
                    in_space = 1;
                    cols++;
                }
            }
        }
    }
    rows--; // NOTE: last row is ops
    rewind(f);

    int *grid = calloc(cols * rows, sizeof(int));
    char *ops = malloc(cols * sizeof(char));
    int y = 0;
    while(fgets(line, sizeof(line), f))
    {
        int x = 0;
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
        {
            line[len-1] = '\0';
            len--;
        }

        if (line[0] == '*' || line[0] == '+')
        {
            for (size_t i = 0; i <= len; i++)
            {
                if (line[i] != '*' && line[i] != '+') continue;
                ops[x++] = line[i];
            }
            break;
        }
        else
        {
            // NOTE: puzzle data range is 0-9999
            char token[5];
            size_t token_idx = 0;
            for (size_t i = 0; i <= len; i++)
            {
                int value = line[i] - 0x30;
                if (value >= 0 && value <= 9)
                {
                    token[token_idx++] = line[i];
                }
                else if (token_idx > 0)
                {
                    token[token_idx++] = '\0';
                    int value = strtol(token, NULL, 10);
                    grid[y * cols + x++] = value;
                    token_idx = 0;
                }
            }
        }
        y++;
    }

    printf("Rows: %zu Cols: %zu\n", rows, cols);
    printf("%s\n", ops);

    uint64_t part_1_sum = part_1(grid, ops, rows, cols);

    printf("Part 1: %" PRIu64 "\n", part_1_sum);

    return 0;
}
