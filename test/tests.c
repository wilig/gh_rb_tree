#include <stdlib.h>
#define GH_RB_TREE_IMPLEMENTATION
#define GH_RB_TREE_VISUALIZE

#include "../gh_rb_tree.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

int cmp(void *k1, void *k2) {
  long ki1 = *(long *)k1;
  long ki2 = *(long *)k2;
  if (ki1 == ki2)
    return 0;
  if (ki1 > ki2)
    return 1;
  return -1;
}

// This is meant to be run with valgrind to find any memory leaks.
int main() {
  srand(time(NULL));

  gh_rb_tree_t *t = gh_rb_tree_create(cmp, free);
  printf("Inserting 100,000 random numbers into a test tree\n");
  for (int i = 0; i < 100000; i++) {
    long *key = malloc(sizeof(long));
    long *value = malloc(sizeof(long));
    *key = (rand() % 100000) + 1;
    *value = *key;
    if (!gh_rb_tree_insert(t, key, value)) {
      // Insert failed, key already present
      free(key);
      free(value);
    }
  }

  printf("Deleting 20,000 random numbers from a test tree\n");
  for (int i = 0; i < 20000; i++) {
    long random_number = (rand() % 100000) + 1;
    void *value = gh_rb_tree_delete(t, &random_number);
    if (value != NULL) {
      free(value);
    }
  }

  gh_rb_tree_destroy(t, gh_rb_tree_free_data);
}
