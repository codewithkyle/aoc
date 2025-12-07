#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef struct {
    size_t len;
    size_t *arr;
} Beams;

typedef struct {
    size_t x;
    size_t y;
} Pos;

typedef struct {
    int x;
    int y;
    int next[2];
    size_t next_len;
} Node;

void add_beam(Beams *beams, size_t x, size_t y, size_t width)
{
    // NOTE: skipping cap checks since
    // we know max from input data
    size_t value = y * width + x;
    beams->arr[beams->len++] = value;
}

void remove_beam_at(Beams *beams, size_t idx)
{
    size_t last = beams->len-1;
    if (idx != last)
    {
        beams->arr[idx] = beams->arr[last];
    }
    beams->len--;
}

void march_beams(Beams *beams, size_t width)
{
    size_t len = beams->len;
    size_t *arr = beams->arr;
    for (size_t i = 0; i < len; i++)
    {
        arr[i] += width;
    }
}

Pos beam_pos(size_t value, size_t width)
{
    return (Pos){
        .x = value % width,
        .y = value / width,
    };
}

int compar(const void *a, const void *b)
{
    const size_t *pa = a;
    const size_t *pb = b;
    if (*pa < *pb) return -1;
    if (*pa > *pb) return 1;
    return 0;
}

void dedupe_beams(Beams *beams)
{
    if (beams->len == 0) return;

    size_t *arr = beams->arr;
    size_t write = 1;
    for (size_t i = 1; i < beams->len; i++)
    {
        if (arr[i] != arr[write-1])
        {
            arr[write++] = arr[i];
        }
    }
    beams->len = write;
}

int part_1(char *grid, Beams *beams, size_t rows, size_t cols, size_t start_offset)
{
    int total = 0;

    // bootstrap
    add_beam(beams, start_offset, 0, cols);

    for (size_t y = 0; y < rows; y++)
    {
        march_beams(beams, cols);

        size_t len = beams->len;
        size_t *arr = beams->arr;
        for (size_t b = len; b-- > 0;)
        {
            Pos pos = beam_pos(arr[b], cols);
            char cell = grid[pos.y * cols + pos.x];
            if (cell == '^')
            {
                remove_beam_at(beams, b);
                if (pos.x+1 < cols)
                {
                    add_beam(beams, pos.x+1, pos.y, cols);
                }
                if (pos.x > 0)
                {
                    add_beam(beams, pos.x-1, pos.y, cols);
                }
                total++;
            }
        }

        qsort(beams->arr, beams->len, sizeof *beams->arr, compar);
        dedupe_beams(beams);
    }

    return total;
}

int find_next_node_below(int x, int y, char *grid, int *node_ids, size_t rows, size_t cols)
{
    for (size_t yy = y+1; yy < rows; yy++)
    {
        size_t idx = yy * cols + x;
        if (grid[idx] == '^')
        {
            return node_ids[idx];
        }
    }

    return -1;
}

void build_edges(char *grid, Node *nodes, int *node_ids, size_t node_count, size_t rows, size_t cols)
{
    for (size_t id = 0; id < node_count; id++)
    {
        int x = nodes[id].x;
        int y = nodes[id].y;
        nodes[id].next_len = 0;

        if (x > 0)
        {
            int left_target = find_next_node_below(x-1, y, grid, node_ids, rows, cols);
            nodes[id].next[nodes[id].next_len++] = left_target;
        }
        if ((size_t)(x+1) < cols)
        {
            int right_target = find_next_node_below(x+1, y, grid, node_ids, rows, cols);
            nodes[id].next[nodes[id].next_len++] = right_target;
        }
    }
}

uint64_t count_from_node(int id, Node *nodes, uint64_t *memo)
{
    if (id < 0) return 1;
    if (memo[id] != 0) return memo[id];

    Node *n = &nodes[id];
    if (n->next_len == 0)
    {
        memo[id] = 1;
        return 1;
    }

    uint64_t total = 0;
    for (size_t i = 0; i < n->next_len; i++)
    {
        int nxt = n->next[i];
        total += count_from_node(nxt, nodes, memo);
    }
    memo[id] = total;
    return total;
}

uint64_t part_2(char *grid, Node *nodes, int *node_ids, size_t node_count, size_t rows, size_t cols, size_t start_offset)
{
    build_edges(grid, nodes, node_ids, node_count, rows, cols);
    int start_node = find_next_node_below((int)start_offset, 0, grid, node_ids, rows, cols);
    if (start_node < 0)
    {
        return 0;
    }
    uint64_t *memo = calloc(node_count, sizeof *memo);
    uint64_t result = count_from_node(start_node, nodes, memo);
    free(memo);
    return result;
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
    size_t num_of_splitters = 0;
    while (fgets(line, sizeof(line), f))
    {
        if (cols == 0)
        {
            size_t len = strlen(line);

            // NOTE: skipping newline check
            // it's there trust me bro
            len--;

            cols = len;
        }
        for (size_t x = 0; x < cols; x++)
        {
            if (line[x] == '^') 
            {
                num_of_splitters++;
            }
        }
        rows++;
    }
    rewind(f);

    char *grid = malloc(rows * cols);
    int *node_ids = malloc(rows * cols * sizeof *node_ids);
    memset(node_ids, 0xFF, rows * cols * sizeof *node_ids);
    Node *nodes = malloc(num_of_splitters * sizeof *nodes);
    size_t y = 0;
    size_t start_offset = 0;
    int node_id_count = 0;
    while (fgets(line, sizeof(line), f))
    {
        // NOTE: lines always have at least min cols
        // lines are never empty
        // trust me bro
        for (size_t x = 0; x < cols; x++)
        {
            size_t idx = y * cols + x;
            grid[idx] = line[x];
            if (line[x] == '^') 
            {
                int id = node_id_count++;
                node_ids[idx] = id;
                nodes[id].x = (int)x;
                nodes[id].y = (int)y;
                nodes[id].next[0] = -1;
                nodes[id].next[1] = -1;
                nodes[id].next_len = 0;
            }
            if (line[x] == 'S') start_offset = x;
        }
        y++;
    }
    size_t max_beams = num_of_splitters * 2;

    // NOTE: malloc can fail but idc
    Beams beams = (Beams){
        .len = 0,
        .arr = malloc(max_beams * sizeof *beams.arr)
    };

    printf("Part 1: %i\n", part_1(grid, &beams, rows, cols, start_offset));
    printf("Part 2: %" PRIu64 "\n", part_2(grid, nodes, node_ids, num_of_splitters, rows, cols, start_offset));

    fclose(f);
    free(beams.arr);
    free(grid);
    free(nodes);
    free(node_ids);
    return 0;
}
