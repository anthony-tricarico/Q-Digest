#include "../include/qcore.h"
#include "../include/queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

void insert_all_nodes(struct QDigest *dest, struct QDigestNode *src) {
    if (!src) return;
    if (src->count > 0) insert_node(dest, src);
    insert_all_nodes(dest, src->left);
    insert_all_nodes(dest, src->right);
}

/* Helper: print a separator */
void print_sep(const char *msg) {
    printf("\n==== %s ====\n", msg);
}

/* Test log_2_ceil function */
void test_log_2_ceil() {
    print_sep("Testing log_2_ceil");
    assert(log_2_ceil(0) == 0);
    assert(log_2_ceil(1) == 0);
    assert(log_2_ceil(2) == 1);
    assert(log_2_ceil(3) == 2);
    assert(log_2_ceil(4) == 2);
    assert(log_2_ceil(5) == 3);
    printf("log_2_ceil tests passed\n");
}

/* Test node creation and deletion */
void test_node_create_delete() {
    print_sep("Testing create_node and delete_node");
    struct QDigestNode *n = create_node(0, 10);
    assert(n != NULL);
    assert(n->count == 0);
    assert(n->left == NULL && n->right == NULL && n->parent == NULL);
    assert(n->lower_bound == 0 && n->upper_bound == 10);
    delete_node(n);
    printf("Node creation/deletion passed\n");
}

/* Test QDigest creation and deletion */
void test_qdigest_create_delete() {
    print_sep("Testing QDigest create/delete");
    struct QDigestNode *root = create_node(0, 10);
    struct QDigest *q = create_q(root, 1, 0, 5, 0);
    assert(q != NULL);
    assert(q->root == root);
    assert(q->num_nodes == 1);
    delete_qdigest(q); // also frees root
    printf("QDigest create/delete passed\n");
}

/* Test basic insertion and percentile */
void test_insert_and_percentile() {
    print_sep("Testing insert and percentile");
    struct QDigest *q = create_tmp_q(5, 15);
    insert(q, 5, 1, false);
    insert(q, 7, 1, false);
    insert(q, 3, 1, false);
    assert(q->N == 3);
    size_t p50 = percentile(q, 0.5);
    printf("Approx 50th percentile: %lu\n", p50);
    delete_qdigest(q);
}

/* Test insert_node and postorder traversal */
void test_insert_node_and_traversal() {
    print_sep("Testing insert_node and traversal");
    struct QDigest *q1 = create_tmp_q(5, 15);
    struct QDigest *q2 = create_tmp_q(5, 15);
    insert(q2, 2, 1, false);
    insert(q2, 4, 1, false);
    insert_all_nodes(q1, q2->root);
    assert(q1->N == 2);
    preorder_to_string(q1->root);
    delete_qdigest(q1);
    delete_qdigest(q2);
}

/* Test expand_tree */
void test_expand_tree() {
    print_sep("Testing expand_tree");
    struct QDigest *q = create_tmp_q(5, 3);
    insert(q, 1, 1, false);
    insert(q, 3, 1, false);
    expand_tree(q, 8); // expand upper bound
    insert(q, 7, 1, false);
    preorder_to_string(q->root);
    delete_qdigest(q);
}

/* Test compress_if_needed */
void test_compress() {
    print_sep("Testing compress_if_needed");
    struct QDigest *q = create_tmp_q(1, 7); // small K to trigger compression easily
    for (size_t i = 0; i < 10; i++) insert(q, i, 1, true);
    print_tree(q);
    delete_qdigest(q);
}

/* Test merge */
void test_merge() {
    print_sep("Testing merge");
    struct QDigest *q1 = create_tmp_q(5, 7);
    struct QDigest *q2 = create_tmp_q(5, 7);
    insert(q1, 1, 1, false);
    insert(q1, 3, 1, false);
    insert(q2, 2, 1, false);
    insert(q2, 4, 1, false);
    merge(q1, q2);
    printf("After merge, q1 N = %lu\n", q1->N);
    preorder_to_string(q1->root);
    delete_qdigest(q1);
    delete_qdigest(q2);
}

/* Test swap_q */
void test_swap_q() {
    print_sep("Testing swap_q");
    struct QDigest *q1 = create_tmp_q(5, 3);
    struct QDigest *q2 = create_tmp_q(10, 7);
    swap_q(q1, q2);
    assert(q1->K == 10 && q2->K == 5);
    assert(q1->root->upper_bound == 7 && q2->root->upper_bound == 3);
    delete_qdigest(q1);
    delete_qdigest(q2);
}

int main(void) {
    test_log_2_ceil();
    test_node_create_delete();
    test_qdigest_create_delete();
    test_insert_and_percentile();
    test_insert_node_and_traversal();
    test_expand_tree();
    test_compress();
    test_merge();
    test_swap_q();

    printf("\nAll tests completed successfully.\n");

    // Check for leaks automatically
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "leaks %d", getpid());
    system(cmd);

    return 0;
}
