#include "include/rb-tree.h"

#include <stdlib.h>
#include <assert.h>

inline void Transplant(RbNode* old_node, RbNode* new_node, RbRoot* root) {
    assert(old_node != NULL);
    if (old_node == root->rb_node) {
        assert(old_node->parent == NULL);
        root->rb_node = new_node;
    } else if (old_node->parent->left == old_node) {
        old_node->parent->left = new_node;
    } else if (old_node->parent->right == old_node) {
        old_node->parent->right = new_node;
    }
    if (new_node) {
        new_node->parent = old_node->parent;
    }
}

inline void RotateLeft(RbNode* x, RbRoot* root) {
    /*
        Before rotate.
                p
                |
                x
               / \
              /   \
             a     y
                  / \
                 /   \
               (b)    c

        After rotate. Besides x and y, we only need to adjust b's parent.
                p
                |
                y
               / \
              /   \
             x     c
            / \
           /   \
          a    (b)
    */
    assert(x != NULL);
    RbNode* y = x->right;
    assert(y != NULL);
    RbNode* b = y->left;
    // Reset y's parent.
    Transplant(x, y, root);
    // Reconnect x with y.
    y->left = x;
    x->parent = y;
    // Reconnect b with x.
    x->right = b;
    if (b) b->parent = x;
}

inline void RotateRight(RbNode* x, RbRoot* root) {
    /*
        Before rotate.
                p
                |
                x
               / \
              /   \
             y     c
            / \
           /   \
          a    (b)

        After rotate. Besides x and y, we only need to adjust b's parent.
                p
                |
                y
               / \
              /   \
             a     x
                  / \
                 /   \
                (b)   c
    */
    assert(x != NULL);
    RbNode* y = x->left;
    assert(y != NULL);
    RbNode* b = y->right;
    // Reset y's parent.
    Transplant(x, y, root);
    // Reconnect x with y.
    y->right = x;
    x->parent = y;
    // Connect b with x;
    x->left = b;
    if (b) b->parent = x;
}

void FixupAfterInsert(RbNode* node, RbRoot* root) {
    assert(node != NULL);

    RbNode* uncle = NULL;
    RbNode* parent = NULL;
    RbNode* gparent = NULL;

    while (IsRed(parent = node->parent)) {
        assert(node->color == kRed);

        gparent = parent->parent;
        assert(gparent != NULL);
        
        if (parent == gparent->left) {
            uncle = gparent->right;
            /*
                Case 1: Uncle node is red.
                In this case, we don't care whether `node` is a left child or a right child.

                        |                              |
                    [gparent]                       gparent
                       / \                            / \
                      /   \                          /   \
                     /     \           ====>        /     \
                  parent  uncle                 [parent] [uncle]
                   /
                  /
                node

            */
            if (IsRed(uncle)) {
                SetColor(gparent, kRed);
                SetColor(parent, kBlack);
                SetColor(uncle, kBlack);
                node = gparent;
                continue;
            }
            /*
                Case 2: Uncle is black, and `node` is the right child of its parent.
                Let's covert this case into Case 3.

                        |                                                 |
                    [gparent]                                         [gparent]
                       / \                                               / \
                      /   \                                             /   \
                     /     \          ====>                            /     \
                  parent [uncle]              `parent` pointer --->  node  [uncle]
                     \                                                /
                      \                                              /
                       \                                            /
                       node                 `node` pointer --->  parent
            */
            if (node == parent->right) {
                RotateLeft(parent, root);
                RbNode* tmp = parent;
                parent = node;
                node = tmp;
            }
            /*
                Case 3: Uncle is black, and `node` is the left child of its parent.
                After rotating and recoloring, the fixup algorithm is finished.

                        |                              |
                    [gparent]                       [parent]
                       / \                            / \
                      /   \                          /   \
                     /     \           ====>        /     \
                  parent  [uncle]                 node  gparent
                    /                                       \
                   /                                         \
                  /                                           \
                node                                        [uncle]
            */
            RotateRight(gparent, root);
            SetColor(parent, kBlack);
            SetColor(gparent, kRed);
            break;
        } 
        else {
            uncle = gparent->left;
            /* Case 1 */
            if (IsRed(uncle)) {
                SetColor(gparent, kRed);
                SetColor(parent, kBlack);
                SetColor(uncle, kBlack);
                node = gparent;
                continue;
            }
            /* Case 2 */
            if (node == parent->left) {
                RotateRight(parent, root);
                RbNode* tmp = parent;
                parent = node;
                node = tmp;
            }
            /* Case 3 */
            RotateLeft(gparent, root);
            SetColor(parent, kBlack);
            SetColor(gparent, kRed);
            break;
        }
    }
    // Don't forget to force to set root node to black.
    SetColor(root->rb_node, kBlack);
}

void InsertIntoRbTree(RbNode* node, RbNode* parent, RbNode** parent_link, RbRoot* root) {
    assert(node != NULL && parent_link != NULL);
    assert(*parent_link == NULL);
    assert(!parent || (&parent->left == parent_link || &parent->right == parent_link));

    node->left = node->right = NULL;
    SetColor(node, kRed);
    node->parent = parent;
    *parent_link = node;

    FixupAfterInsert(node, root);
}

void FixupAfterRemove(RbNode* node, RbNode* node_parent, RbRoot* root) {
    while ((node == NULL || IsBlack(node)) && node != root->rb_node) {
        assert(node_parent != NULL);
        assert(node_parent->left == node || node_parent->right == node);
        assert(node == NULL || node->parent == node_parent);

        RbNode* sibling = NULL;
        if (node == node_parent->left) {
            sibling = node_parent->right;
            assert(sibling != NULL);
            /*
                Case 1: The `node` has a red sibling.
                Let's call `RotateLeft` so that we can enter case 2, 3 or 4.

                       |                                        |
                    [parent]                                [sibling]
                      / \                                      / \
                     /   \                                    /   \
                    /     \                                  /     \
                   /       \                                /       \
               [[node]]   sibling         ====>           parent     [R]
                  /\       / \                            / \       / \
                 /  \     /   \                          /   \     /   \
                 a  b    /     \                        /     \    e   f
                       [L]     [R]                 [[node]]   [L]
                       / \     / \                    / \     / \
                      /   \   /   \                  /   \   /   \
                      c   d   e   f                  a   b   c   d
            */
            if (IsRed(sibling)) {
                assert(IsBlack(node_parent));
                RotateLeft(node_parent, root);
                SetColor(node_parent, kRed);
                SetColor(sibling, kBlack);
            }
            /*
                Case 2: The `node` has a black sibling, and the sibling hasn't any red child.

                       |                                        |
                   <parent>                                 [<parent>]   <----`node` pointer
                      / \                                      / \
                     /   \                                    /   \
                    /     \                                  /     \
                   /       \                                /       \
               [[node]]  [sibling]         ====>         [node]   sibling
                 / \       / \                            / \       / \
                /   \     /   \                          /   \     /   \
                a   b    /     \                         a   b    /     \
                       [L]     [R]                               [L]    [R]
                       / \     / \                               / \    / \
                      /   \   /   \                             /   \  /   \
                      c   d   e   f                             c   d  e   f
            */
            else if (!IsRed(sibling->left) && !IsRed(sibling->right)) {
                SetColor(sibling, kRed);
                node = node_parent;
                node_parent = node->parent;
            }
            /*
                Case 3: The `node` has a black sibling, and the sibling has a red child in left but not right.
                Let's call `RotateRight` so that we can convert it into Case 4.

                       |                                        |
                   <parent>                                 <parent>
                      / \                                      / \
                     /   \                                    /   \
                    /     \                                  /     \
                   /       \                                /       \
               [[node]]  [sibling]         ====>        [[node]]    [L]  <------ `sibling` pointer in next loop
                 / \       / \                            / \       / \
                /   \     /   \                          /   \     /   \
                a   b    /     \                         a   b    /     \
                        L      [R]                               c    sibling
                       / \     / \                                      / \
                      /   \   /   \                                    /   \
                      c   d   e   f                                   /     \
                                                                     d      [R]
                                                                            / \
                                                                           /   \
                                                                           e   f
            */
            else if (IsBlack(sibling->right)) {
                assert(IsRed(sibling->left));
                SetColor(sibling, kRed);
                SetColor(sibling->left, kBlack);
                RotateRight(sibling, root);
            }
            /*
                Case 4: The `node` has a black sibling, and the sibling's right child is red.
                Let's call `RotateLeft` so that we can convert it into Case 4.

                       |                                        |
                    <parent>                                <sibling>
                      / \                                      / \
                     /   \                                    /   \
                    /     \                                  /     \
                   /       \                                /       \
               [[node]]  [sibling]         ====>        [parent]    [R]
                 / \       / \                            / \       / \
                /   \     /   \                          /   \     /   \
                a   b    /     \                        /     \    e   f
                       {L}      R                    [node]   {L}
                       / \     / \                    / \     / \
                      /   \   /   \                  /   \   /   \
                      c   d   e   f                  a   b   c   d
            */
            else {
                SetColor(sibling, node_parent->color);
                SetColor(node_parent, kBlack);
                SetColor(sibling->right, kBlack);
                RotateLeft(node_parent, root);
                // Ok, now the rb-tree is rebalanced.
                // Let's exit the loop simply.
                break;
            }
        }
        else {
            sibling = node_parent->left;
            assert(sibling != NULL);
            /* Case 1: The `node` has a red sibling. */
            if (IsRed(sibling)) {
                assert(IsBlack(node_parent));
                RotateRight(node_parent, root);
                SetColor(node_parent, kRed);
                SetColor(sibling, kBlack);
            }
            /* Case 2 */
            else if (!IsRed(sibling->left) && !IsRed(sibling->right)) {
                SetColor(sibling, kRed);
                node = node_parent;
                node_parent = node->parent;
            }
            /* Case 3 */
            else if (IsBlack(sibling->left)) {
                assert(IsRed(sibling->right));
                SetColor(sibling, kRed);
                SetColor(sibling->right, kBlack);
                RotateLeft(sibling, root);
            }
            /* Case 4 */
            else {
                SetColor(sibling, node_parent->color);
                SetColor(node_parent, kBlack);
                SetColor(sibling->left, kBlack);
                RotateRight(node_parent, root);
                // Ok, now the rb-tree is rebalanced.
                // Let's exit the loop simply.
                break;
            }
        }
    }
    // If `node` is tree root, it will "absorb" additional black color.
    // If `node` is red, it will be recolored to black simply.
    if (node != NULL) {
        SetColor(node, kBlack);
    }
}

void RemoveFromRbTree(RbNode* node, RbRoot* root) {
    RbNode* replacement = NULL;
    RbNode* replacement_parent = node->parent;
    Color removed_color = node->color;
    if (node->left == NULL) {
        replacement = node->right;
        Transplant(node, replacement, root);
    }
    else if (node->right == NULL) {
        replacement = node->left;
        Transplant(node, replacement, root);
    } 
    else {
        // Search the successor of node.
        RbNode* successor = node->right;
        while (successor->left != NULL) {
            successor = successor->left;
        }
        // Record the replacement of `successor`,
        // which will replace `successor` in logical.
        replacement = successor->right;
        if (successor != node->right) {
            // Reset the parent of successor's right child
            replacement_parent = successor->parent;
            Transplant(successor, replacement, root);
            // Reconnect node's right child with successor.
            successor->right = node->right;
            node->right->parent = successor;
        } else {
            replacement_parent = successor;
        }
        // Replace node with successor.
        Transplant(node, successor, root);
        // Reconnect node's left child with successor.
        successor->left = node->left;
        node->left->parent = successor;
        // Record the original color of successor,
        // which is the real color the rb-tree lost.
        removed_color = successor->color;
        // Recolor successor with node's color.
        SetColor(successor, node->color);
    }
    // Don't forget to rebalance the rb-tree.
    if (removed_color == kBlack) {
        FixupAfterRemove(replacement, replacement_parent, root);
    }
}
