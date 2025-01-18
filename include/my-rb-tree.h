#ifndef MY_RB_TREE_H_
#define MY_RB_TREE_H_

#include <vector>

#include "include/rb-tree.h"

struct MyData {
    int value;
    struct RbNode rb_node;
};

/* Basic operations for rb-tree */
void MyInsertIntoRbTree(MyData* new_data, RbRoot* root);

void MyRemoveFromRbTree(MyData* data, RbRoot* root);

void MyPrintRbTree(RbNode* node);

/* Test tools for rb-tree */
bool IsLegalRbTree(RbRoot* root);

bool RbTreeTesterAuto(bool print_log = false);

bool RbTreeTesterWithValues(const std::vector<int>&, bool print_log = false);

#endif  // MY_RB_TREE_H_
