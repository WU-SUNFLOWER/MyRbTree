#ifndef RB_TREE_H_
#define RB_TREE_H_

#include <stdlib.h>
#include <stdbool.h>

typedef enum { kBlack, kRed } Color;

typedef struct RbNode {
    Color color;
    struct RbNode* parent;
    struct RbNode* left;
    struct RbNode* right;
} RbNode;

typedef struct RbRoot {
    RbNode* rb_node;
} RbRoot;

#define InitializedRbRoot { NULL, }

#define OffsetOf(type, member) ((uintptr_t)(&((type*)0)->member))

#define ContainerOf(ptr, type, member) ((type*)((uintptr_t)(ptr) - OffsetOf(type, member)))

#ifdef __cplusplus
extern "C" {
#endif

    /* Small utils */
    inline void SetColor(RbNode* node, Color color) {
        node->color = color;
    }

    inline bool IsRed(RbNode* node) {
        return node != NULL && node->color == kRed;
    }

    inline bool IsBlack(RbNode* node) {
        return node == NULL || node->color == kBlack;
    }

    inline bool IsEmptyRbRoot(RbRoot* root) {
        return root->rb_node == NULL;
    }

    inline void Transplant(RbNode* old_node, RbNode* new_node, RbRoot* root);  // Same with CLRS

    /* Rotation function. */
    inline void RotateLeft(RbNode* x, RbRoot* root);

    inline void RotateRight(RbNode* x, RbRoot* root);

    /* Insert implementation */
    void FixupAfterInsert(RbNode* node, RbRoot* root);

    void InsertIntoRbTree(RbNode* node, RbNode* parent, RbNode** parent_link, RbRoot* root);

    /* Remove implementation */
    void FixupAfterRemove(RbNode* node, RbNode* node_parent, RbRoot* root);

    void RemoveFromRbTree(RbNode* node, RbRoot* root);

#ifdef __cplusplus
}
#endif

#endif  // RB_TREE_H_
