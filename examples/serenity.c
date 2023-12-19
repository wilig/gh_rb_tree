#define _POSIX_C_SOURCE 200809L
#define GH_RB_TREE_IMPLEMENTATION

#include "../gh_rb_tree.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct shipmate_data_t {
  char *job;
  int combat_experience;
  float years_aboard;
} shipmate_data_t;

void node_key_printer(void *key, void *value) { printf("%s\n", (char *)key); }

int main() {
  gh_rb_tree_t *t =
      gh_rb_tree_create((gh_rb_tree_node_comparator_t *)strcmp, free);

  gh_rb_tree_insert(t, strdup("Mal"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Zoe"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Wash"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Inara"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Jayne"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Kaylee"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Simon"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("River"), malloc(sizeof(shipmate_data_t)));
  gh_rb_tree_insert(t, strdup("Book"), malloc(sizeof(shipmate_data_t)));

  printf("The Crew:\n");
  gh_rb_tree_visit(t, node_key_printer);

  // :-(
  free(gh_rb_tree_delete(t, "Wash"));

  shipmate_data_t *data = gh_rb_tree_search(t, "Mal");
  assert(data != NULL);

  data = gh_rb_tree_search(t, "Ravageur");
  assert(data == NULL);

  printf("First crew member: %p\n", gh_rb_tree_minimum(t));
  printf("Last crew member: %p\n", gh_rb_tree_maximum(t));

  gh_rb_tree_destroy(t, gh_rb_tree_free_data);
}
