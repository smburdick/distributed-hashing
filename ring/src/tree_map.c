/**
 * Server map implemented as a red-black tree
 * Based on CLRS Chap. 13
 */

#include <stdlib.h>
#include <stdio.h>
#include "tree_map.h"

cache_id succ(rbt_ptr t, hash_value value)
{
    node_ptr curr = t->root;
    if (value >= rbt_max(t, curr)->hv)
        return rbt_min(t, curr)->cid;
    return recur_succ(t, t->root, t->root, value);
}

void print(rbt_ptr t, node_ptr c)
{
    if (c == t->nil) return;
    printf("Hash value (curr, left, right): %d %d %d\n", c->hv, c->left->hv, c->right->hv);
    print(t, c->right);
    print(t, c->left);
}

cache_id recur_succ(rbt_ptr t, node_ptr root, node_ptr suc, hash_value value)
{
    if (root == t->nil) {
        return suc->cid;
    }
    if (value == root->hv) {
        /* return leftmost node of right subtree */
        suc = root->right;
        if (suc == t->nil) {
            suc = root;
            while (suc->parent != t->nil && suc->hv < value) {
                suc = suc->parent;
            }
            return suc->cid;
        }
        while (suc->left != t->nil) {
            suc = suc->left;
        }
        return suc->cid;
    }
    if (root->hv > value) {
        suc = root;
        return recur_succ(t, root->left, suc, value);
    }
    else {
        return recur_succ(t, root->right, suc, value);
    }

}

node_ptr rbt_max(rbt_ptr t, node_ptr x)
{
    while (x->right != t->nil)
        x = x->right;
    return x;
}

rbt_ptr new_rbt(void)
{
    rbt_ptr r;
    r = (rbt_ptr) malloc(sizeof(struct rbt));
    r->nil = (node_ptr) malloc(sizeof(struct node));
    r->nil->color = BLACK;
    r->nil->hv = 0;
    r->nil->cid = 0;
    r->root = r->nil;
    r->size = 0;
    return r;
}

node_ptr new_node(rbt_ptr t, cache_id cid, hash_value hv, rbt_node_color color)
{

    node_ptr n;

    n = (node_ptr) malloc(sizeof(struct node));
    n->color = color;

    n->color = color;
    n->hv = hv;

    n->parent = t->nil;
    n->left = t->nil;
    n->right = t->nil;
    n->cid = cid;


    return n;
}

void left_rotate(rbt_ptr t, node_ptr x)
{
    node_ptr y = x->right;
    x->right = y->left;
    if (y->left != t->nil)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == t->nil)
        t->root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void right_rotate(rbt_ptr t, node_ptr y)
{
    node_ptr x = y->left;
    y->left = x->right;
    if (x->right != t->nil)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == t->nil)
        t->root = x;
    else if (y == y->parent->right)
        y->parent->right = x;
    else
        y->parent->left = x;
    x->right = y;
    y->parent = x;
}

void rbt_insert(rbt_ptr t, node_ptr z)
{
    t->size++;
    node_ptr x = t->root;
    node_ptr y = t->nil;
    while (x != t->nil) {
        y = x;
        if (z->hv < x->hv)
            x = x->left;
        else
            x = x->right;
    }
    z->parent = y;
    if (y == t->nil)
        t->root = z;
    else if (z->hv < y->hv)
        y->left = z;
    else
        y->right = z;
    z->left = t->nil;
    z->right = t->nil;
    z->color = RED;
    rbt_insert_fixup(t, z);
}

void rbt_insert_fixup(rbt_ptr t, node_ptr z)
{
    node_ptr y;
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(t, z->parent->parent);
            }
        }
        else {
            y = z->parent->parent->left;
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(t, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(t, z->parent->parent);
            }
        }
    }
    t->root->color = BLACK;
}

void rbt_transplant(rbt_ptr t, node_ptr u, node_ptr v)
{
    if (u->parent == t->nil) {
        t->root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else
        u->parent->right = v;
    v->parent = u->parent;
}

node_ptr rbt_min(rbt_ptr t, node_ptr x)
{
    while (x->left != t->nil)
        x = x->left;
    return x;
}

void rbt_delete(rbt_ptr t, node_ptr z)
{
    t->size--;
    node_ptr x, y = z;
    rbt_node_color y_orig_col = y->color;
    if (z->left == t->nil) {
        x = z->right;
        rbt_transplant(t, z, z->right);
    }
    else if (z->right == t->nil) {
        x = z->left;
        rbt_transplant(t, z, z->left);
    }
    else {
        y = rbt_min(t, z->right);
        y_orig_col = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        }
        else {
            rbt_transplant(t, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rbt_transplant(t, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color;
    }
    if (y_orig_col == BLACK)
        rbt_delete_fixup(t, x);
    free(z);
}

void rbt_delete_fixup(rbt_ptr t, node_ptr x)
{
    node_ptr w;
    while (x != t->root && x->color == BLACK) {
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                left_rotate(t, x->parent);
                w = x->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(t, w);
                    w = x->parent->right;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                left_rotate(t, x->parent);
                x = t->root;
            }
        }
        else {
            w = x->parent->left;
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                right_rotate(t, x->parent);
                w = x->parent->left;
            }
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            }
            else {
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    left_rotate(t, w);
                    w = x->parent->left;
                }
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                right_rotate(t, x->parent);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

void recur_free_rbt(rbt_ptr t, node_ptr n)
{
    if (n->left != t->nil) recur_free_rbt(t, n->left);
    if (n->right != t->nil) recur_free_rbt(t, n->right);
    printf("Freeing %d\n", n->hv);
    free(n);
}

void free_rbt(rbt_ptr t)
{
    recur_free_rbt(t, t->root);
    free(t->nil);
    free(t);
}
