//
//           Red Black Tree Implementation v1.0  (wilig 2023)
//
// There is no warranty implied.  Use at your own risk.
//
// Sources:
//
// Introduction to Algorithms - Cormen, Leiserson, Rivest
//
// https://www.youtube.com/watch?v=qvZGUFHWChY - Michael Sambol
// https://www.youtube.com/watch?v=nMExd4DthdA - Rob Edwards
//
// Inspiration
//
// https://github.com/etherealvisage/avl
//
// Disclaimer:
//
// Any bugs are due to an imperfect implementation.
//
//
// Copyright:
//
// This code is placed in the public domain.  Do with it what you
// please.  If you find it worthwhile I'd appreciate an
// attribution or some nice tea.
//

// If no assert is defined, use the default assert
#ifndef GH_ASSERT
#include <assert.h>

#define GH_ASSERT(s) assert(s)
#endif // GH_ASSERT

// If no allocator is defined use the default malloc/free
#ifndef GH_ALLOCATOR
#include <stdlib.h>

#define GH_ALLOCATOR(sz) malloc(sz)
#define GH_FREE(p) free(p)
#endif // GH_ALLOCATOR

#ifndef GHIDEF
#ifdef GH_RB_TREE_STATIC
#define GHIDEF static
#else
#define GHIDEF extern
#endif // GH_RB_TREE_STATIC
#endif // GHIDEF

// Function prototypes for comparing two keys, should return
//  Negative value - if key1 is less than key2
//  0 - if the keys are equal
//  Positive value - if key1 is greater then key2
typedef int(gh_rb_tree_node_comparator_t)(void *key1, void *key2);
typedef void(gh_rb_tree_node_key_deallocator_t)(void *key);
typedef void(gh_rb_tree_node_visitor_t)(void *key, void *value);

typedef struct gh_rbt__node_t {
  struct gh_rbt__node_t *left, *right, *parent;
  void *key;
  void *value;
  enum { BLACK, RED } color;
} gh_rbt__node_t;

typedef struct gh_rb_tree_t {
  gh_rbt__node_t *root;
  gh_rb_tree_node_comparator_t *comparator;
  gh_rb_tree_node_key_deallocator_t *key_deallocator;
} gh_rb_tree_t;

GHIDEF gh_rb_tree_t *
gh_rb_tree_create(gh_rb_tree_node_comparator_t comparator,
                  gh_rb_tree_node_key_deallocator_t key_deallocator);
GHIDEF void gh_rb_tree_free(gh_rb_tree_t *t);
GHIDEF void *gh_rb_tree_search(gh_rb_tree_t *t, void *key);
GHIDEF int gh_rb_tree_insert(gh_rb_tree_t *t, void *key, void *value);
GHIDEF void *gh_rb_tree_delete(gh_rb_tree_t *t, void *key);
GHIDEF void *gh_rb_tree_maximum(gh_rb_tree_t *t);
GHIDEF void *gh_rb_tree_minimum(gh_rb_tree_t *t);
GHIDEF void gh_rb_tree_visit(gh_rb_tree_t *t, gh_rb_tree_node_visitor_t func);
GHIDEF void gh_rb_tree_destroy(gh_rb_tree_t *t, gh_rb_tree_node_visitor_t func);
GHIDEF void gh_rb_tree_free_data(void *key, void *value);

#ifdef GH_RB_TREE_IMPLEMENTATION
// A stand in for nil to simplify conditionals
static gh_rbt__node_t gh_rbt__tnill = {0};

static gh_rbt__node_t *create_node(gh_rb_tree_t *tree, void *key, void *value) {
  gh_rbt__node_t *node = GH_ALLOCATOR(sizeof(gh_rbt__node_t));
  node->key = key;
  node->value = value;
  node->color = RED;
  node->left = node->right = node->parent = &gh_rbt__tnill;
  return node;
}

static gh_rbt__node_t *gh_rbt__find_node(gh_rb_tree_t *tree,
                                         gh_rbt__node_t *node, void *key) {
  // Set cmp_res to non-zero to start the while loop
  int cmp_res = -1;
  while (node != &gh_rbt__tnill && cmp_res != 0) {
    cmp_res = tree->comparator(key, node->key);
    if (cmp_res < 0) {
      node = node->left;
    } else if (cmp_res > 0) {
      node = node->right;
    }
  }
  return node;
}

static int gh_rbt__insert_node(gh_rb_tree_node_comparator_t *cmp,
                               gh_rbt__node_t *parent, gh_rbt__node_t *node) {
  while (1) {
    int cmp_res = cmp(parent->key, node->key);
    if (cmp_res == 0) {
      return 0;
    } else if (cmp_res > 0) {
      if (parent->left == &gh_rbt__tnill) {
        parent->left = node;
        node->parent = parent;
        return 1;
      } else
        parent = parent->left;
    } else if (cmp_res < 0) {
      if (parent->right == &gh_rbt__tnill) {
        parent->right = node;
        node->parent = parent;
        return 1;
      } else
        parent = parent->right;
    }
  }
}

static void gh_rbt__rotate_left(gh_rb_tree_t *tree, gh_rbt__node_t *node) {
  gh_rbt__node_t *sibling = node->right;
  node->right = sibling->left;

  // Turn sibling's left subtree into node's right subtree
  if (sibling->left != &gh_rbt__tnill) {
    sibling->left->parent = node;
  }
  sibling->parent = node->parent;
  if (node->parent == &gh_rbt__tnill) {
    // Set sibling to root of the tree
    tree->root = sibling;
  } else {
    if (node == node->parent->left) {
      node->parent->left = sibling;
    } else {
      node->parent->right = sibling;
    }
  }
  sibling->left = node;
  node->parent = sibling;
}

static void gh_rbt__rotate_right(gh_rb_tree_t *tree, gh_rbt__node_t *node) {
  gh_rbt__node_t *sibling = node->left;
  node->left = sibling->right;

  // Turn sibling's right subtree into node's left subtree
  if (sibling->right != &gh_rbt__tnill) {
    sibling->right->parent = node;
  }
  sibling->parent = node->parent;
  if (node->parent == &gh_rbt__tnill) {
    tree->root = sibling;
  } else {
    if (node == node->parent->right) {
      node->parent->right = sibling;
    } else {
      node->parent->left = sibling;
    }
  }
  sibling->right = node;
  node->parent = sibling;
}

static void gh_rbt__rebalance_after_insert(gh_rb_tree_t *tree,
                                           gh_rbt__node_t *node) {
  gh_rbt__node_t *uncle;
  while (node != tree->root && node->parent->color == RED) {
    // Rebalancing cases
    if (node->parent->parent != &gh_rbt__tnill &&
        node->parent == node->parent->parent->left) {
      uncle = node->parent->parent->right;
      if (uncle != &gh_rbt__tnill && uncle->color == RED) {
        // Case 1:  Just recolor the nodes
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->right) {
          // Case 2
          node = node->parent;
          gh_rbt__rotate_left(tree, node);
        }
        // Case 3
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        gh_rbt__rotate_right(tree, node->parent->parent);
      }
    } else if (node->parent->parent != &gh_rbt__tnill) {
      uncle = node->parent->parent->left;
      if (uncle != &gh_rbt__tnill && uncle->color == RED) {
        // Case 1:  Just recolor the nodes
        node->parent->color = BLACK;
        uncle->color = BLACK;
        node->parent->parent->color = RED;
        node = node->parent->parent;
      } else {
        if (node == node->parent->left) {
          // Case 2
          node = node->parent;
          gh_rbt__rotate_right(tree, node);
        }
        // Case 3
        node->parent->color = BLACK;
        node->parent->parent->color = RED;
        gh_rbt__rotate_left(tree, node->parent->parent);
      }
    }
  }
  tree->root->color = BLACK;
}

static gh_rbt__node_t *gh_rbt__minimum_node(gh_rbt__node_t *node) {
  while (node->left != &gh_rbt__tnill) {
    node = node->left;
  }
  return node;
}

gh_rbt__node_t *gh_rbt__maximum_node(gh_rbt__node_t *node) {
  while (node->right != &gh_rbt__tnill) {
    node = node->right;
  }
  return node;
}

static gh_rbt__node_t *gh_rbt__successor(const gh_rbt__node_t *node) {
  GH_ASSERT(node != NULL);
  if (node->left->color == RED) {
    return gh_rbt__maximum_node(node->left);
  } else if (node->right != &gh_rbt__tnill) {
    return gh_rbt__minimum_node(node->right);
  }
  gh_rbt__node_t *parent = node->parent;
  while (parent != &gh_rbt__tnill && node == parent->right) {
    node = parent;
    parent = node->parent;
  }
  return parent;
}

static void gh_rbt__visit_each_node(gh_rbt__node_t *node,
                                    gh_rb_tree_node_visitor_t f) {
  if (node == &gh_rbt__tnill)
    return;
  gh_rbt__visit_each_node(node->left, f);
  f(node->key, node->value);
  gh_rbt__visit_each_node(node->right, f);
}

static void gh_rbt__rebalance_after_deletion(gh_rb_tree_t *tree,
                                             gh_rbt__node_t *node) {
  gh_rbt__node_t *sibling;
  while (tree->root != node && node->color == BLACK) {
    if (node == node->parent->left) {
      sibling = node->parent->right;
      if (sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        gh_rbt__rotate_left(tree, node->parent);
        sibling = node->parent->right;
      }
      if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
        sibling->color = RED;
        node = node->parent;
      } else {
        if (sibling->right->color == BLACK) {
          sibling->color = RED;
          sibling->left->color = BLACK;
          gh_rbt__rotate_right(tree, sibling);
          sibling = node->parent->right;
        }
        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        sibling->right->color = BLACK;
        gh_rbt__rotate_left(tree, node->parent);
        node = tree->root;
      }
    } else {
      sibling = node->parent->left;
      if (sibling->color == RED) {
        sibling->color = BLACK;
        node->parent->color = RED;
        gh_rbt__rotate_right(tree, node->parent);
        sibling = node->parent->left;
      }
      if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
        sibling->color = RED;
        node = node->parent;
      } else {
        if (sibling->left->color == BLACK) {
          sibling->color = RED;
          sibling->right->color = BLACK;
          gh_rbt__rotate_left(tree, sibling);
          sibling = node->parent->left;
        }
        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        sibling->left->color = BLACK;
        gh_rbt__rotate_right(tree, node->parent);
        node = tree->root;
      }
    }
  }
  node->color = BLACK;
}

static gh_rbt__node_t *gh__rb_tree_delete(gh_rb_tree_t *tree, void *key) {
  gh_rbt__node_t *child;
  gh_rbt__node_t *successor;

  gh_rbt__node_t *node_to_delete = gh_rbt__find_node(tree, tree->root, key);
  if (node_to_delete == &gh_rbt__tnill)
    return node_to_delete;

  if (node_to_delete->left == &gh_rbt__tnill ||
      node_to_delete->right == &gh_rbt__tnill) {
    successor = node_to_delete;
  } else {
    successor = gh_rbt__successor(node_to_delete);
  }
  if (successor->left != &gh_rbt__tnill) {
    child = successor->left;
  } else {
    child = successor->right;
  }
  child->parent = successor->parent;
  if (successor->parent == &gh_rbt__tnill) {
    tree->root = child;
  } else if (successor == successor->parent->left) {
    successor->parent->left = child;
  } else {
    successor->parent->right = child;
  }
  if (successor != node_to_delete) {
    // Swap the the key and value pointers so they can be freed correctly.
    // This mimics the copy specified in the original algorithm
    void *deleted_key = node_to_delete->key;
    void *deleted_value = node_to_delete->value;
    node_to_delete->key = successor->key;
    node_to_delete->value = successor->value;
    successor->key = deleted_key;
    successor->value = deleted_value;
  }
  if (successor->color == BLACK) {
    gh_rbt__rebalance_after_deletion(tree, child);
  }
  return successor;
}

// Public Interface
//
//
// Create a new red black tree.  Takes a comparator that will be used
// to evaulate order of the tree keys stored in the tree, and a deallocator
// for freeing keys when an item is deleted from the tree.
GHIDEF gh_rb_tree_t *
gh_rb_tree_create(gh_rb_tree_node_comparator_t comparator,
                  gh_rb_tree_node_key_deallocator_t key_deallocator) {
  gh_rb_tree_t *tree = GH_ALLOCATOR(sizeof(gh_rb_tree_t));
  tree->root = &gh_rbt__tnill;
  tree->comparator = comparator;
  tree->key_deallocator = key_deallocator;
  return tree;
}

GHIDEF void *gh_rb_tree_delete(gh_rb_tree_t *tree, void *key) {
  gh_rbt__node_t *node = gh__rb_tree_delete(tree, key);
  if (node == &gh_rbt__tnill) {
    return NULL;
  }

  void *value = node->value;
  tree->key_deallocator(node->key);
  GH_FREE(node);
  return value;
}

GHIDEF int gh_rb_tree_insert(gh_rb_tree_t *tree, void *key, void *value) {
  gh_rbt__node_t *node = create_node(tree, key, value);
  if (tree->root == &gh_rbt__tnill) {
    tree->root = node;
  } else {
    if (!gh_rbt__insert_node(tree->comparator, tree->root, node)) {
      free(node);
      return 0;
    }
  }
  // Enforce red black tree rules
  gh_rbt__rebalance_after_insert(tree, node);
  return 1;
}

GHIDEF void *gh_rb_tree_search(gh_rb_tree_t *t, void *key) {
  gh_rbt__node_t *node = gh_rbt__find_node(t, t->root, key);
  if (node == &gh_rbt__tnill) {
    return NULL;
  } else {
    return node->value;
  }
}

GHIDEF void *gh_rb_tree_maximum(gh_rb_tree_t *t) {
  gh_rbt__node_t *node = gh_rbt__maximum_node(t->root);
  return node->value;
}

GHIDEF void *gh_rb_tree_minimum(gh_rb_tree_t *t) {
  gh_rbt__node_t *node = gh_rbt__minimum_node(t->root);
  return node->value;
}

GHIDEF void gh_rb_tree_visit(gh_rb_tree_t *t, gh_rb_tree_node_visitor_t func) {
  gh_rbt__visit_each_node(t->root, func);
}

GHIDEF void gh_rb_tree_destroy(gh_rb_tree_t *t,
                               gh_rb_tree_node_visitor_t destroyer) {

  // Free the keys and data
  gh_rb_tree_visit(t, destroyer);
  // Free the tree nodes
  gh_rbt__node_t *node = t->root;
  gh_rbt__node_t *tmp;
  while (node != &gh_rbt__tnill) {
    if (node->left == &gh_rbt__tnill && node->right == &gh_rbt__tnill) {
      if (node->parent->left == node)
        node->parent->left = &gh_rbt__tnill;
      else
        node->parent->right = &gh_rbt__tnill;
      tmp = node;
      node = node->parent;
      free(tmp);
    } else if (node->left != &gh_rbt__tnill) {
      node = node->left;
    } else {
      node = node->right;
    }
  }
  // Free the tree
  GH_FREE(t);
}

GHIDEF void gh_rb_tree_free_data(void *key, void *value) {
  GH_FREE(key);
  GH_FREE(value);
}

#endif // GH_RB_TREE_IMPLEMENTATION

#ifdef GH_RB_TREE_VISUALIZE
/*

 The trees can be visualized via graphviz by
    #define GH_RB_TREE_VISUALIZE
 before the library is imported.

 and calling
    gh_rb_tree_to_graphvis(tree, fh, key-conversion-fn)

 This will generate a graphvis textual representation
 of the red black tree that can be used to generate an
 image using graphvis's `dot` command line tool.

*/

#include <stdio.h>

typedef char *(gh_rb_tree_visualize_key_to_printable)(void *key);

static void
gh_rbt__node_to_graphvis(gh_rbt__node_t *node, FILE *file,
                         gh_rb_tree_visualize_key_to_printable convert) {

  if (node->left && node->left != &gh_rbt__tnill) {
    fprintf(file, "\t%s -> %s\n", convert(node->key), convert(node->left->key));
    gh_rbt__node_to_graphvis(node->left, file, convert);
  }
  if (node->right && node->right != &gh_rbt__tnill) {
    fprintf(file, "\t%s -> %s\n", convert(node->key),
            convert(node->right->key));
    gh_rbt__node_to_graphvis(node->right, file, convert);
  }
}

static void
gh_rbt__write_node_attributes(gh_rbt__node_t *node, FILE *file,
                              gh_rb_tree_visualize_key_to_printable convert) {
  if (node != &gh_rbt__tnill) {
    fprintf(file,
            "%s [style=\"filled\" fontname=\"Arial\" fontcolor=\"white\" "
            "fillcolor=\"%s\"]\n",
            convert(node->key), node->color == RED ? "red" : "black");
    if (node->left) {
      gh_rbt__write_node_attributes(node->left, file, convert);
    }

    if (node->right) {
      gh_rbt__write_node_attributes(node->right, file, convert);
    }
  }
}

GHIDEF void
gh_rb_tree_to_graphvis(gh_rb_tree_t *rbt, FILE *file,
                       gh_rb_tree_visualize_key_to_printable convert) {
  fprintf(file, "digraph RBT {\n");

  if (!rbt)
    fprintf(file, "\n");
  else if (rbt->root->right == &gh_rbt__tnill &&
           rbt->root->left == &gh_rbt__tnill)
    fprintf(file, "\t%s;\n", convert(rbt->root->key));
  else {
    gh_rbt__write_node_attributes(rbt->root, file, convert);
    gh_rbt__node_to_graphvis(rbt->root, file, convert);
  }

  fprintf(file, "}\n");
}

#endif // GH_RB_TREE_VISUALIZE
