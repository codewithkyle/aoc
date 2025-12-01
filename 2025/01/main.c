#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

uint turn_dial(int start_at, char dir, uint amount)
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
        dial_at = turn_dial(dial_at, dir, amount);
        if (dial_at == 0)
        {
            times_at_zero++;
        }
    }
    fclose(f);
    printf("%d", times_at_zero);
    return 0;
}
