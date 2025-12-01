#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

typedef struct {
    uint dial_at;
    uint times_at_zero;
} DialResult;

DialResult turn_dial(int dial_at, char dir, uint amount)
{
    uint times_at_zero = 0;
    int mult = 1;
    if (dir == 'R' || dir == 'r')
    {
        mult = -1;
    }
    for (size_t i = 1; i <= amount; i++)
    {
        dial_at += (1 * mult);
        if (dial_at < 0)
        {
            dial_at = 99;
        }
        else if (dial_at > 99)
        {
            dial_at = 0;
        }
        if (dial_at == 0)
        {
            times_at_zero++;
        }
    }
    DialResult r = {
        .dial_at = (uint)dial_at,
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
    uint dial_at = 50;
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
