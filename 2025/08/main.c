#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

enum { MAX_NODES = 1000 };
enum { MAX_CIRCUTS = 500 };
enum { CONN_TO_MAKE = 1000 }; // 10 for sample, 1000 for puzzle

typedef struct {
    int x, y, z;
    int circut_idx;
} Node;

typedef struct {
    size_t len;
    size_t cap;
    size_t *node_ids;
} Circut;

typedef struct {
    size_t a;
    size_t b;
    uint64_t dist2;  // squared distance
} Edge;

size_t create_circut(Circut *circuts)
{
    for (size_t i = 0; i < MAX_CIRCUTS; i++)
    {
        if (circuts[i].len == 0)
        {
            return i;
        }
    }
    return SIZE_MAX;
}

void circut_realloc_nodes(Circut *circut)
{
    // NOTE: I know this can fail, it won't
    // trust me bro
    size_t new_cap = circut->cap*2;
    circut->node_ids = realloc(circut->node_ids, new_cap * sizeof *circut->node_ids);
    circut->cap = new_cap;
}

void circut_add_node(Circut *circut, size_t node_idx)
{
    if (circut->len == circut->cap)
    {
        circut_realloc_nodes(circut);
    }
    circut->node_ids[circut->len++] = node_idx;
}

void circut_merge(Circut *a, Circut *b)
{
    size_t old_len = a->len;
    size_t new_len = a->len + b->len;
    while (new_len >= a->cap)
    {
        circut_realloc_nodes(a);
    }

    for (size_t i = 0; i < b->len; i++)
    {
        a->node_ids[old_len+i] = b->node_ids[i];
    }
    a->len = new_len;
    b->len = 0;
}

void circut_sync_nodes(Circut *circut, int circut_idx, Node *nodes)
{
    size_t len = circut->len;
    size_t *arr = circut->node_ids;
    for (size_t i = 0; i < len; i++)
    {
        size_t node_idx = arr[i];
        nodes[node_idx].circut_idx = circut_idx;
    }
}

static inline uint64_t dist(Node *a, Node *b)
{
    uint64_t dx = a->x - b->x;
    uint64_t dy = a->y - b->y;
    uint64_t dz = a->z - b->z;
    return (uint64_t)(dx*dx + dy*dy + dz*dz);
}

int edge_compar(const void *pa, const void *pb)
{
    const Edge *ea = pa;
    const Edge *eb = pb;

    if (ea->dist2 < eb->dist2) return -1;
    if (ea->dist2 > eb->dist2) return 1;
    return 0;
}

int compar(const void *a, const void *b)
{
    const Circut *ca = a;
    const Circut *cb = b;

    if (ca->len > cb->len) return -1;
    if (ca->len < cb->len) return 1;
    return 0;
}

uint64_t part_1(Circut *circuts, Node *nodes, Edge *edges, size_t edge_count)
{
    size_t edges_used = 0;
    for (size_t e = 0; e < edge_count && edges_used < CONN_TO_MAKE; ++e)
    {
        size_t i = edges[e].a;
        size_t j = edges[e].b;

        Node *a = &nodes[i];
        Node *b = &nodes[j];

        if (a->circut_idx != -1 && a->circut_idx == b->circut_idx)
        {
            edges_used++;
            continue;
        }
        else if (a->circut_idx != -1 && b->circut_idx == -1)
        {
            Circut *circut = &circuts[a->circut_idx];
            circut_add_node(circut, j);
            b->circut_idx = a->circut_idx;
        }
        else if (a->circut_idx == -1 && b->circut_idx != -1)
        {
            Circut *circut = &circuts[b->circut_idx];
            circut_add_node(circut, i);
            a->circut_idx = b->circut_idx;
        }
        else if (a->circut_idx != -1 && b->circut_idx != -1)
        {
            Circut *keep = &circuts[a->circut_idx];
            Circut *kill = &circuts[b->circut_idx];
            circut_merge(keep, kill);
            circut_sync_nodes(keep, a->circut_idx, nodes);
        }
        else
        {
            size_t circut_idx = create_circut(circuts);
            if (circut_idx == SIZE_MAX) {
                perror("no empty circuits");
                exit(1);
            }
            Circut *circut = &circuts[circut_idx];
            circut_add_node(circut, i);
            circut_add_node(circut, j);
            a->circut_idx = (int)circut_idx;
            b->circut_idx = (int)circut_idx;
        }

        edges_used++;
    }

    qsort(circuts, MAX_CIRCUTS, sizeof *circuts, compar);
    return circuts[0].len * circuts[1].len * circuts[2].len;
}

uint64_t part_2(Circut *circuts, Node *nodes, Edge *edges, size_t edge_count)
{
    size_t merge_count = 0;
    uint64_t part_2_answer = 0;

    for (size_t e = 0; e < edge_count; ++e)
    {
        size_t i = edges[e].a;
        size_t j = edges[e].b;

        Node *a = &nodes[i];
        Node *b = &nodes[j];

        if (a->circut_idx != -1 && a->circut_idx == b->circut_idx)
        {
            continue;
        }
        else if (a->circut_idx != -1 && b->circut_idx == -1)
        {
            Circut *circut = &circuts[a->circut_idx];
            circut_add_node(circut, j);
            b->circut_idx = a->circut_idx;
            merge_count++;
        }
        else if (a->circut_idx == -1 && b->circut_idx != -1)
        {
            Circut *circut = &circuts[b->circut_idx];
            circut_add_node(circut, i);
            a->circut_idx = b->circut_idx;
            merge_count++;
        }
        else if (a->circut_idx != -1 && b->circut_idx != -1)
        {
            Circut *keep = &circuts[a->circut_idx];
            Circut *kill = &circuts[b->circut_idx];
            circut_merge(keep, kill);
            circut_sync_nodes(keep, a->circut_idx, nodes);
            merge_count++;
        }
        else
        {
            size_t circut_idx = create_circut(circuts);
            if (circut_idx == SIZE_MAX)
            {
                perror("no empty circuits");
                exit(1);
            }
            Circut *circut = &circuts[circut_idx];
            circut_add_node(circut, i);
            circut_add_node(circut, j);
            a->circut_idx = (int)circut_idx;
            b->circut_idx = (int)circut_idx;
            merge_count++;
        }

        if (merge_count == MAX_NODES-1)
        {
            part_2_answer = (uint64_t)nodes[i].x * (uint64_t)nodes[j].x;
            break;
        }
    }
    return part_2_answer;
}

int main()
{
    FILE *f = fopen("data.txt", "r");
    if (!f)
    {
        perror("failed to open file");
        return 1;
    }

    Node *nodes = malloc(MAX_NODES * sizeof *nodes);
    char line[4096];
    size_t idx = 0;
    while(fgets(line, sizeof(line), f))
    {
        size_t len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
        {
            line[len-1] = '\0';
            len--;
        }
        int x,y,z;
        x = y = z = -1;
        char *token = strtok(line, ",");
        while (token != NULL)
        {
            int value = (int)strtoul(token, NULL, 10);
            if (x < 0) x = value;
            else if (y < 0) y = value;
            else if (z < 0)
            {
                z = value;
                break;
            }
            token = strtok(NULL, ",");
        }
        nodes[idx++] = (Node){
            .x = x,
            .y = y,
            .z = z,
            .circut_idx = -1,
        };
    }

    Circut *circuts = malloc(MAX_CIRCUTS * sizeof *circuts);
    for (size_t i = 0; i < MAX_CIRCUTS; i++)
    {
        circuts[i] = (Circut) {
            .cap = 16,
            .len = 0,
            .node_ids = malloc(16 * sizeof *circuts->node_ids),
        };
    }

    size_t edge_count = 0;
    Edge *edges = malloc(MAX_NODES * (MAX_NODES - 1) / 2 * sizeof *edges);
    for (size_t i = 0; i < MAX_NODES; i++)
    {
        for (size_t j = i + 1; j < MAX_NODES; j++)
        {
            edges[edge_count++] = (Edge){
                .a = i,
                .b = j,
                .dist2 = dist(&nodes[i], &nodes[j]),
            };
        }
    }
    qsort(edges, edge_count, sizeof *edges, edge_compar);

    printf("Part 1: %" PRIu64 "\n", part_1(circuts, nodes, edges, edge_count));

    // NOTE: reset for part 2
    for (size_t i = 0; i < MAX_NODES; i++) {
        nodes[i].circut_idx = -1;
    }
    for (size_t i = 0; i < MAX_CIRCUTS; i++) {
        circuts[i].len = 0;
    }
    printf("Part 2: %" PRIu64 "\n", part_2(circuts, nodes, edges, edge_count));

    free(circuts);
    free(nodes);
    free(edges);
    fclose(f);
    return 0;
}
