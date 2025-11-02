#include "../../include/qcore.h"
#include "../../include/memory_utils.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

/* This function is used to compute the base-2 logarithm of a given input n */
size_t log_2_ceil(size_t n) {
  /* Check if the given n is already a power of 2 with bitwise operations */
  bool is_pow2 = (n & -n) == n;
  /* Initialize return value to 0 */
  int res = 0;
  /* While n is greater than 1 increase result by 1 */
  while (n > 1) {
    n /= 2;
    res++;
  }
  /* Return the result as is if n was a power of 2, otherwise add 1 to it */
  return res + (is_pow2 ? 0 : 1);
}

/* Declare QDigestNode, the building block of the Data Structure */
struct QDigestNode {
  // initialize pointers to left, right and parent nodes
  struct QDigestNode *left, *right, *parent;
  size_t count;
  // variables to store the upper and lower bound of the node.
  // Range contained is [min, max], so that both bounds are included.
  size_t lower_bound, upper_bound;
};

/* This function allocates memory for a node and initializes some of its
 * members. */
struct QDigestNode *create_node(size_t lower_bound, size_t upper_bound) {
  struct QDigestNode *ret = xmalloc(sizeof(struct QDigestNode));

  // make all pointers NULL pointers
  ret->left = ret->right = ret->parent = NULL;
  // make count start from 0
  ret->count = 0;

  // assign both a lower and upper bound to respective struct members
  ret->lower_bound = lower_bound;
  ret->upper_bound = upper_bound;

  return ret;
}

/* This function deletes a node and frees the memory that was allocated to it */
void delete_node(struct QDigestNode *n) { free(n); }

/* Declare QDigest structure */
struct QDigest {
  // pointer to the node representing the root
  struct QDigestNode *root;

  size_t num_nodes;
  size_t N, K;
  size_t num_inserts;
};

/* TODO: implement the create_q function similar to what done above */
struct QDigest *create_q(struct QDigestNode *root, size_t num_nodes, size_t N,
                         size_t K, size_t num_inserts) {
  struct QDigest *ret = xmalloc(sizeof(struct QDigest));
  ret->root = root;
  ret->num_nodes = num_nodes;
  ret->N = N;
  ret->K = K;
  ret->num_inserts = num_inserts;

  return ret;
}

/* Constructor for a special case used inside the expand_tree function */
struct QDigest *create_tmp_q(size_t K, size_t upper_bound) {
  struct QDigest *tmp = xmalloc(sizeof(struct QDigest));
  struct QDigestNode *n = create_node(0, upper_bound);
  tmp->root = n;
  tmp->num_nodes = 1;
  tmp->N = 0;
  tmp->K = K;
  return tmp;
}
/* This function returns the aggregated count for a node and its siblings. */
size_t node_and_sibling_count(struct QDigestNode *n) {
  size_t ret = n->count;

  // check if the pointer is not NULL (i.e., that node exists) and then update
  // count
  if (n->left)
    ret += n->left->count;
  if (n->right)
    ret += n->left->count;

  return ret;
}

/* Determines which tree nodes can be deleted.
 * A tree node which has a count of 0 can be deleted only if it has no children.
 *
 * Returns 'true' or 'false' depending on whether it deleted the node n from the
 * tree.
 * TODO: check if memory allocation logic is correct */
bool delete_node_if_needed(struct QDigest *q, struct QDigestNode *n, int level,
                           int l_max) {
  if (n->count == 0 && (!n->left && !n->right)) {
    if (n->parent->left == n) {
      n->parent->left = NULL;
    } else {
      n->parent->right = NULL;
    }

    delete_node(n);
    (q->num_nodes)--;
    return true;
  }
  return false;
}

void compress(struct QDigest *q, struct QDigestNode *n, int level, int l_max,
              size_t nDivk) {
  if (!n)
    return;

  compress(q, n->left, level + 1, l_max, nDivk);
  compress(q, n->right, level + 1, l_max, nDivk);

  if (level > 0) {
    bool deleted = delete_node_if_needed(q, n, level, l_max);
    if (!deleted && node_and_sibling_count(n->parent) < nDivk) {
      struct QDigestNode *par = n->parent;
      par->count = node_and_sibling_count(par);

      if (par->left) {
        par->left->count = 0;
        delete_node_if_needed(q, par->left, level, l_max);
      }
      if (par->right) {
        par->right->count = 0;
        delete_node_if_needed(q, par->right, level, l_max);
      }

    } // if (!deleted && ...)
  } // if (level > 0)
}

/* TODO: implement print_tree */
void print_tree() { ; }

/* TODO: implement swap function which should mirror the std::swap from C++ */
void swap_q(struct QDigest *q1, struct QDigest *q2) {
  struct QDigest *tmp = xmalloc(sizeof(struct QDigest));
  tmp = q1;
  q1 = q2;
  q2 = tmp;
  free(tmp);
}

void expand_tree(struct QDigest *q, size_t upper_bound) {
  assert(upper_bound - 1 > q->root->upper_bound);
  // check that the upper_bound is a power of 2
  assert((upper_bound & (-upper_bound)) == upper_bound);

  upper_bound--;

  struct QDigest *tmp = create_tmp_q(q->K, upper_bound);

  if (q->N == 0) {
    swap_q(q, tmp);
    return;
  }

  const bool try_compress = false;
  // TODO: continue from here
}
