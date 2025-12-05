#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct {
    int dial_at;
    uint times_at_zero;
} DialResult;

DialResult turn_dial_part_2(int dial_at, char dir, uint amount)
{
    int mult = (dir == 'R' || dir == 'r') ? -1 : 1;
    int offset = mult * (int)amount;
    int pos = dial_at + offset;
    pos %= 100;
    if (pos < 0)
    {
        pos += 100;
    }

    uint times_at_zero = 0;
    if (amount > 0)
    {
        if (mult > 0)
        {
            times_at_zero = (dial_at + amount) / 100;
        }
        else
        {
            if (dial_at == 0)
            {
                times_at_zero = amount / 100;
            }
            else if (amount < dial_at)
            {
                times_at_zero = 0;
            }
            else
            {
                times_at_zero = 1 + (amount - dial_at) / 100;
            }
        }
    }

    DialResult r = {
        .dial_at = pos,
        .times_at_zero = times_at_zero,
    };
    return r;
}

uint part_2(FILE *f)
{
    int dial_at = 50;
    uint times_at_zero = 0;
    char line[4096];
    while (fgets(line, sizeof(line), f))
    {
        char dir = line[0];
        uint amount = strtoul(&line[1], NULL, 10);
        DialResult r = turn_dial_part_2(dial_at, dir, amount);
        dial_at = r.dial_at;
        times_at_zero += r.times_at_zero;
    }
    return times_at_zero;
}

uint turn_dial_part_1(int start_at, char dir, uint amount)
{
    int mult = 1;
    if (dir == 'R' || dir == 'r')
    {
        mult = -1;
    }
    for (size_t i = 1; i <= amount; i++)
    {
        start_at += (1 * mult);
        if (start_at < 0)
        {
            start_at = 99;
        }
        else if (start_at > 99)
        {
            start_at = 0;
        }
    }
    return (uint)start_at;
}

uint part_1(FILE *f)
{
    uint times_at_zero = 0;
    int dial_at = 50;
    char line[4096];
    while (fgets(line, sizeof(line), f))
    {
        char dir = line[0];
        uint amount = strtoul(&line[1], NULL, 10);
        dial_at = turn_dial_part_1(dial_at, dir, amount);
        if (dial_at == 0)
        {
            times_at_zero++;
        }
    }
    return times_at_zero;
}

int main()
{
    FILE *f = fopen("data.txt", "r");
    if (!f)
    {
        perror("fopen");
        return -1;
    }
    uint times_at_zero_part_1 = part_1(f);
    rewind(f);
    uint times_at_zero_part_2 = part_2(f);
    fclose(f);
    printf("Part 1: %u\n", times_at_zero_part_1);
    printf("Part 2: %u\n", times_at_zero_part_2);
    return 0;
}
