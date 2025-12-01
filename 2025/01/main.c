#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct {
    int dial_at;
    uint times_at_zero;
} DialResult;

DialResult turn_dial(int dial_at, char dir, uint amount)
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

int main()
{
    FILE *f = fopen("data.txt", "r");
    if (!f)
    {
        perror("fopen");
        return -1;
    }
    uint times_at_zero = 0;
    int dial_at = 50;
    char line[4096];
    while (fgets(line, sizeof(line), f))
    {
        char dir = line[0];
        uint amount = strtoul(&line[1], NULL, 10);
        DialResult r = turn_dial(dial_at, dir, amount);
        times_at_zero += r.times_at_zero;
        dial_at = r.dial_at;
    }
    fclose(f);
    printf("%d", times_at_zero);
    return 0;
}
