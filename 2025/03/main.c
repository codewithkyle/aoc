#include <assert.h>
#include <stdio.h>
#include <string.h>

int main()
{
    FILE* file = fopen("data.txt", "r");
    if (!file)
    {
        perror("failed to open file");
        return -1;
    }

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
            int value = line[i] - 0x30;
            if (value > left)
            {
                left_idx = i;
                left = value;
            }
        }

        for (int i = left_idx+1; i < len; i++)
        {
            int value = line[i] - 0x30;
            if (value > right)
            {
                right = value;
            }
        }

        joltage_sum += left * 10 + right;
    }
    printf("%i\n", joltage_sum);

    return 0;
}
