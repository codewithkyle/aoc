#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

uint64_t part_2(char *dg, char *ops_row, size_t rows, size_t width)
{
    uint64_t total = 0;

    size_t x = 0;
    while (x < width)
    {
        int sep = 1;

        if (ops_row[x] == '+' || ops_row[x] == '*')
        {
            sep = 0;
        }
        else
        {
            for (size_t y = 0; y < rows; y++)
            {
                if (dg[y * width + x] != ' ')
                {
                    sep = 0;
                    break;
                }
            }
        }

        if (sep)
        {
            x++;
            continue;
        }

        size_t start = x;
        size_t end   = x;

        while (end + 1 < width)
        {
            size_t nx = end + 1;
            int next_sep = 1;

            if (ops_row[nx] == '+' || ops_row[nx] == '*')
            {
                next_sep = 0;
            }
            else
            {
                for (size_t y = 0; y < rows; y++)
                {
                    if (dg[y * width + nx] != ' ')
                    {
                        next_sep = 0;
                        break;
                    }
                }
            }

            if (next_sep) break;
            end++;
        }

        char op = '+';
        for (size_t cx = start; cx <= end; cx++)
        {
            if (ops_row[cx] == '+' || ops_row[cx] == '*')
            {
                op = ops_row[cx];
                break;
            }
        }

        uint64_t acc = 0;
        int acc_init = 0;
        for (size_t cx = start; cx <= end; cx++)
        {
            uint64_t value = 0;
            int digit_seen = 0;

            for (size_t y = 0; y < rows; y++)
            {
                char ch = dg[y * width + cx];
                if (ch >= '0' && ch <= '9')
                {
                    value = value * 10 + (uint64_t)(ch - 0x30);
                    digit_seen = 1;
                }
            }

            if (!digit_seen) continue;

            if (!acc_init)
            {
                acc = value;
                acc_init = 1;
            }
            else if (op == '*')
            {
                acc *= value;
            }
            else
            {
                acc += value;
            }
        }

        total += acc;
        x = end + 1;
    }
    return total;
}

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
    size_t cols = 0;
    size_t rows = 0;
    uint8_t in_space = 1;
    size_t width = 0;

    while (fgets(line, sizeof(line), f))
    {
        rows++;

        if (cols == 0)
        {
            size_t len = strlen(line);
            if (len > 0 && line[len-1] == '\n')
            {
                len--;
                line[len] = '\0';
            }
            width = len;

            for (size_t i = 0; i < len; i++)
            {
                int value = line[i] - '0';
                if (value >= 0 && value <= 9)
                {
                    if (in_space)
                    {
                        cols++;
                        in_space = 0;
                    }
                }
                else
                {
                    in_space = 1;
                }
            }
        }
    }

    rows--;  // last row is ops
    rewind(f);

    int *grid = calloc(cols * rows, sizeof(int));
    char *digits_grid = malloc(rows * width);
    memset(digits_grid, ' ', rows * width);

    char *ops = malloc(cols);
    char *ops_row = malloc(width);
    memset(ops_row, ' ', width);

    size_t y = 0;
    while (fgets(line, sizeof(line), f))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
        {
            len--;
            line[len] = '\0';
        }

        if (line[0] == '*' || line[0] == '+')
        {
            size_t opi = 0;
            for (size_t i = 0; i < len; i++)
            {
                ops_row[i] = line[i];
                if (line[i] == '+' || line[i] == '*') ops[opi++] = line[i];
            }
            break;
        }

        size_t x = 0;
        char token[5];
        size_t token_idx = 0;

        for (size_t i = 0; i < len; i++)
        {
            digits_grid[y * width + i] = line[i];

            int v = line[i] - '0';
            if (v >= 0 && v <= 9)
            {
                token[token_idx++] = line[i];
            }
            else if (token_idx > 0)
            {
                token[token_idx] = '\0';
                grid[y * cols + x++] = atoi(token);
                token_idx = 0;
            }
        }

        if (token_idx > 0)
        {
            token[token_idx] = '\0';
            grid[y * cols + x++] = atoi(token);
        }

        y++;
    }

    uint64_t p1 = part_1(grid, ops, rows, cols);
    uint64_t p2 = part_2(digits_grid, ops_row, rows, width);

    printf("Part 1: %" PRIu64 "\n", p1);
    printf("Part 2: %" PRIu64 "\n", p2);

    return 0;
}

