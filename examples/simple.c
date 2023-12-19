#define GH_RB_TREE_IMPLEMENTATION
#define GH_RB_TREE_VISUALIZE

#include "../gh_rb_tree.h"
#include <stdio.h>

char *converter(void *key) {
  long ikey = (long)key;
  char *out = calloc(10, sizeof(char));
  snprintf(out, 10, "%ld", ikey);
  return out;
}

int cmp(void *k1, void *k2) {
  long ki1 = (long)k1;
  long ki2 = (long)k2;
  if (ki1 == ki2)
    return 0;
  if (ki1 > ki2)
    return 1;
  return -1;
}

void dealloc(void *_) {}

int main() {
  gh_rb_tree_t *t = gh_rb_tree_create(cmp, dealloc);

  for (int i = 1; i < 17; i++) {
    gh_rb_tree_insert(t, (void *)(long)i, (void *)(long)i);
  }

  gh_rb_tree_delete(t, (void *)(long)1);
  gh_rb_tree_delete(t, (void *)(long)12);
  gh_rb_tree_delete(t, (void *)(long)2);
  gh_rb_tree_delete(t, (void *)(long)16);
  gh_rb_tree_delete(t, (void *)(long)8);
  gh_rb_tree_delete(t, (void *)(long)9);

  gh_rb_tree_delete(t, (void *)(long)7);
  gh_rb_tree_delete(t, (void *)(long)6);
  gh_rb_tree_delete(t, (void *)(long)13);
  gh_rb_tree_delete(t, (void *)(long)4);

  FILE *file = fopen("rbt_graph.dot", "w");
  assert(file != 0);
  printf("Write graph to rbt_graph.dot\n");
  gh_rb_tree_to_graphvis(t, file, converter);
}
