#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int part_2(int *grid, int rows, int cols)
{
    int total = 0;
    while (1)
    {
        int sum = 0;
        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                size_t idx = y * cols + x;
                if (grid[idx] == 0) continue;
                int adj_sum = 0;
                if (x > 0 && grid[y * cols + (x-1)]) adj_sum++;
                if (x < cols-1 && grid[y * cols + (x+1)]) adj_sum++;

                if (y > 0 && grid[(y-1) * cols + x]) adj_sum++;
                if (y < rows-1 && grid[(y+1) * cols + x]) adj_sum++;

                if (y > 0 && x > 0 && grid[(y-1) * cols + (x-1)]) adj_sum++;
                if (y < rows-1 && x < cols-1 && grid[(y+1) * cols + (x+1)]) adj_sum++;

                if (y > 0 && x < cols-1 && grid[(y-1) * cols + (x+1)]) adj_sum++;
                if (y < rows-1 && x > 0 && grid[(y+1) * cols + (x-1)]) adj_sum++;

                if (adj_sum < 4) 
                {
                    grid[idx] = 0;
                    sum++;
                }
            }
        }
        if (sum == 0) break;
        total += sum;
    }
    return total;
}

int part_1(int *grid, int rows, int cols)
{
    int sum = 0;
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < cols; x++)
        {
            if (grid[y * cols + x] == 0) continue;
            int adj_sum = 0;
            if (x > 0 && grid[y * cols + (x-1)]) adj_sum++;
            if (x < cols-1 && grid[y * cols + (x+1)]) adj_sum++;

            if (y > 0 && grid[(y-1) * cols + x]) adj_sum++;
            if (y < rows-1 && grid[(y+1) * cols + x]) adj_sum++;

            if (y > 0 && x > 0 && grid[(y-1) * cols + (x-1)]) adj_sum++;
            if (y < rows-1 && x < cols-1 && grid[(y+1) * cols + (x+1)]) adj_sum++;

            if (y > 0 && x < cols-1 && grid[(y-1) * cols + (x+1)]) adj_sum++;
            if (y < rows-1 && x > 0 && grid[(y+1) * cols + (x-1)]) adj_sum++;

            if (adj_sum < 4) sum++;
        }
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

    int rows = 0;
    int cols = 0;
    char line[4096];
    while(fgets(line, sizeof(line), f))
    {
        if (cols == 0)
        {
            cols = strlen(line)-1;
        }
        rows++;
    }
    rewind(f);
    int *grid = calloc(cols * rows, sizeof(int));
    int y = 0;
    while(fgets(line, sizeof(line), f))
    {
        for (size_t x = 0; x < cols; x++)
        {
            if (line[x] == '@')
            {
                grid[y * cols + x] = 1;
            }
        }
        y++;
    }

    int part_1_sum = part_1(grid, rows, cols);
    int part_2_sum = part_2(grid, rows, cols);

    fclose(f);
    free(grid);
    printf("Part 1: %i\n", part_1_sum);
    printf("Part 2: %i\n", part_2_sum);

    return 0;
}
