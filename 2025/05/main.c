#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <inttypes.h>

typedef struct {
    uint64_t start;
    uint64_t end;
} Range;

typedef struct {
    uint64_t capacity;
    uint64_t length;
    Range *range;
} Ranges;

void add_range(Ranges *ranges, uint64_t start, uint64_t end)
{
    if (ranges->length == ranges->capacity)
    {
        ranges->capacity *= 2;
        // NOTE: realloc could fail but I'm ignoring that for AoC
        ranges->range = realloc(ranges->range, ranges->capacity * sizeof(Range));
    }

    ranges->range[ranges->length++] = (Range){ start, end };
}

int compar(const void *a, const void *b)
{
    const Range *ra = a;
    const Range *rb = b;

    if (ra->start < rb->start) return -1;
    if (ra->start > rb->start) return 1;
    return 0;
}

void merge_ranges(Ranges *ranges)
{
    if (ranges->length == 0) return;

    size_t out = 0;
    for (size_t i = 1; i < ranges->length; i++)
    {
        Range *curr = &ranges->range[out];
        Range *next = &ranges->range[i];

        if (next->start <= curr->end)
        {
            if (next->end > curr->end)
            {
                curr->end = next->end;
            }
        }
        else
        {
            out++;
            ranges->range[out] = *next;
        }
    }
    ranges->length = out+1;
}

ssize_t binary_search(Ranges *ranges, uint64_t value)
{
    if (ranges->length == 0)
    {
        return -1;
    }

    size_t low_idx = 0;
    size_t high_idx = ranges->length-1;

    while (low_idx <= high_idx)
    {
        size_t mid_idx = low_idx + (high_idx - low_idx) * 0.5;
        const Range *r = &ranges->range[mid_idx];

        if (value < r->start)
        {
            if (mid_idx == 0)
            {
                break;
            }
            high_idx = mid_idx-1;
        }
        else if (value > r->end)
        {
            low_idx = mid_idx+1;
        }
        else
        {
            return (ssize_t)mid_idx;
        }
    }

    return -1;
}

uint64_t part_1(Ranges *ranges, uint64_t item_id)
{
    return binary_search(ranges, item_id) >= 0;
}

uint64_t part_2(Ranges *ranges)
{
    uint64_t sum = 0;
    for (size_t i = 0; i < ranges->length; i++)
    {
        Range *r = &ranges->range[i];
        uint64_t diff = (r->end - r->start) + 1;
        sum += diff;
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

    Ranges ranges = {0};
    ranges.capacity = 16;
    ranges.length = 0;
    ranges.range  = malloc(ranges.capacity * sizeof(Range));

    u_int8_t finished_loading = 0;
    char line[4096];
    uint64_t part_1_sum = 0;
    while (fgets(line, sizeof(line), f))
    {
        size_t len = strlen(line);

        if (!finished_loading)
        {
            if (line[0] == '\n' && len == 1)
            {
                finished_loading = 1;
                qsort(ranges.range, ranges.length, sizeof(Range), compar);
                merge_ranges(&ranges);
                continue;
            }
            char *dash = strchr(line, '-');
            if (dash)
            {
                *dash = '\0';
                char *left = line;
                char *right = dash + 1;
                uint64_t start = strtoull(left, NULL, 10);
                uint64_t end = strtoull(right, NULL, 10);
                add_range(&ranges, start, end);
            }
            continue;
        }

        if (line[len-1] == '\n')
        {
            line[len-1] = '\0';
            len--;
        }

        uint64_t item_id = strtoull(line, NULL, 10);
        part_1_sum += part_1(&ranges, item_id);
    }

    printf("Part 1: %" PRIu64 "\n", part_1_sum);
    printf("Part 2: %" PRIu64 "\n", part_2(&ranges));

    return 0;
}
