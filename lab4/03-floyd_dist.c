#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef long data_type;

int floyd(const data_type **graph, size_t node_num, data_type **dist,
          data_type inf, size_t **prevs) {
  for (size_t i = 0; i < node_num; i++) {
    for (size_t j = 0; j < node_num; j++) {
      if (graph[i][j] != inf) {
        dist[i][j] = graph[i][j];
        if (i != j) {
          prevs[i][j] = i;
        } else {
          prevs[i][j] = SIZE_MAX;
        }
        continue;
      }
      dist[i][j] = inf;
      prevs[i][j] = SIZE_MAX;
    }
  }
  for (size_t from = 0; from < node_num; from++) {
    for (size_t mid = 0; mid < node_num; mid++) {
      for (size_t to = 0; to < node_num; to++) {
        if (dist[from][mid] < inf && dist[mid][to] < inf &&
            dist[mid][to] < inf - dist[from][mid] &&
            dist[from][mid] + dist[mid][to] < dist[from][to]) {
          dist[from][to] = dist[from][mid] + dist[mid][to];
          if (prevs[mid][to] == SIZE_MAX) {
            prevs[from][to] = mid;
          } else {
            prevs[from][to] = prevs[mid][to];
          }
        }
      }
    }
  }
  return 0;
}

int solve() {
  size_t sz;
  scanf("%lu", &sz);
  data_type inf = 1e9;
  data_type **graph = calloc(sz, sizeof(uintptr_t)),
            **dist = calloc(sz, sizeof(uintptr_t));
  size_t **prevs = calloc(sz, sizeof(uintptr_t));
  for (size_t i = 0; i < sz; i++) {
    graph[i] = calloc(sz, sizeof(data_type));
    dist[i] = calloc(sz, sizeof(data_type));
    prevs[i] = calloc(sz, sizeof(size_t));
    for (size_t j = 0; j < sz; j++) {
      graph[i][j] = inf;
      scanf("%ld", &graph[i][j]);
    }
  }

  floyd((const data_type **)graph, sz, dist, inf, prevs);

  size_t qnum;
  scanf("%lu", &qnum);
  for (size_t i = 0; i < qnum; i++) {
    size_t from, to;
    scanf("%lu%lu", &from, &to);
    printf("%ld\n", dist[from][to]);
  }

  for (size_t i = 0; i < sz; i++) {
    free(graph[i]);
    free(prevs[i]);
    free(dist[i]);
  }
  free(graph);
  free(dist);
  free(prevs);
  return 0;
}

int main(void) { solve(); }
