#include "include/my-rb-tree.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <random>
#include <unordered_set>

#include <cstdio>
#include <cassert>

void MyInsertIntoRbTree(MyData* new_data, RbRoot* root) {
    RbNode* parent = nullptr;
    RbNode** link_ptr = &root->rb_node;
    while (*link_ptr) {
        parent = *link_ptr;
        MyData* parent_data = ContainerOf(parent, struct MyData, rb_node);
        if (parent_data->value > new_data->value) {
            link_ptr = &parent->left;
        }
        else {
            link_ptr = &parent->right;
        }
    }
    assert(link_ptr != nullptr);
    InsertIntoRbTree(&new_data->rb_node, parent, link_ptr, root);
}

void MyRemoveFromRbTree(MyData* data, RbRoot* root) {
    RemoveFromRbTree(&data->rb_node, root);
    free(data);
}

void MyPrintRbTree(RbNode* node) {
    assert(node != nullptr);
    if (node->left) MyPrintRbTree(node->left);

    MyData* data = ContainerOf(node, struct MyData, rb_node);
    printf("color=%s value=%d ", node->color == kBlack ? "black" : "red", data->value);
    
    if (node->left) {
        data = ContainerOf(node->left, struct MyData, rb_node);
        printf("left_value=%d ", data->value);
    }
    
    if (node->right) {
        data = ContainerOf(node->right, struct MyData, rb_node);
        printf("right_value=%d ", data->value);
    }
    
    putchar('\n');

    if (node->right) MyPrintRbTree(node->right);
}

/*
    Is your tree a legal rb-tree?
    1. Is this tree a legal BST?
    2. Are all nodes either red or black in color?
    3. Are all red nodes' child and parent node black in color? 
    4. Are the numbers of black nodes in the simple paths from root node to any leaf node same?
    5. Is the root node black in color?
*/

static void InorderTraversalChecker(
    RbNode* node, std::vector<int>* record,
    int cur_black_count, std::vector<int>* black_count_record
) {
    // 2. Are all nodes either red or black in color?
    if (!IsRed(node) && !IsBlack(node)) {
        throw "Failed: All nodes are either red or black in color.";
    }
    // 3. Are all red nodes' child and parent node black in color?
    if (IsRed(node) && (IsRed(node->parent) || IsRed(node->left) || IsRed(node->right))) {
        throw "Failed: All red nodes' child and parent node are black in color.";
    }

    // Update black count
    if (IsBlack(node)) {
        cur_black_count += 1;
    }

    // Inorder traversal left
    if (node->left != nullptr) {
        InorderTraversalChecker(node->left, record, cur_black_count, black_count_record);
    }

    // Record node value
    MyData* my_data_struct = ContainerOf(node, struct MyData, rb_node);
    record->push_back(my_data_struct->value);

    // Inorder traversal right
    if (node->right != nullptr) {
        InorderTraversalChecker(node->right, record, cur_black_count, black_count_record);
    }

    // 4. Are the numbers of black nodes in the simple paths from root node to any leaf node same?
    if (!node->left && !node->right) {
        if (black_count_record->size() > 0 && cur_black_count != black_count_record->back()) {
            throw "Failed: The numbers of black nodes in the"
                  " simple paths from root node to any leaf node are same";
        }
        black_count_record->push_back(cur_black_count);
    }
}

bool IsLegalRbTree(RbRoot* root_node) {
    // 5. Is the root node black in color?
    if (!IsBlack(root_node->rb_node)) {
        std::cerr << "Failed: The root node is black in color" << std::endl;
        return false;
    }

    std::vector<int> record;
    std::vector<int> black_count_record;
    try {
        InorderTraversalChecker(root_node->rb_node, &record, 0, &black_count_record);
    }
    catch (const char* msg) {
        std::cerr << msg << std::endl;
        return false;
    }

    // 1. Is this tree a legal BST?
    std::vector<int> sorted_record = record;
    std::sort(sorted_record.begin(), sorted_record.end());
    if (!std::equal(record.begin(), record.end(), sorted_record.begin())) {
        std::cerr << "Failed: This tree is a legal BST." << std::endl;
        return false;
    }
    
    return true;
}

bool RbTreeTesterAuto(int node_num, bool print_log) {
    std::random_device rd;  // Random seed generator
    std::mt19937 gen(rd());  // Random number generator
    std::uniform_int_distribution<int> dis(-node_num * 2, node_num * 2);  // The range of random number is [-1000, 1000]

    RbRoot root = InitializedRbRoot;
    std::queue<MyData*> datas;
    
    // Test the insert function.
    std::unordered_set<int> set;
    for (int i = 0; i < node_num; ++i) {
        MyData* my_data_struct = reinterpret_cast<MyData*>(malloc(sizeof(MyData)));
        assert(my_data_struct != nullptr);
        do {
            my_data_struct->value = dis(gen);
        } while (set.count(my_data_struct->value) > 0);
        set.insert(my_data_struct->value);
        // Record rb-tree node.
        datas.push(my_data_struct);
        // Insert the new node to the rb-tree.
        MyInsertIntoRbTree(my_data_struct, &root);
    }
    if (print_log) std::cout << "Inserted all nodes." << std::endl;

    // Check whether the rb-tree is legal.
    if (!IsLegalRbTree(&root)) {
        return false;
    }
    if (print_log) std::cout << "Passed check after inserting." << std::endl;

    if (print_log) MyPrintRbTree(root.rb_node);

    // Test the remove function.
    while (!datas.empty()) {
        MyData* data = datas.front();
        int node_value = data->value;

        if (print_log) std::cout << "Removing node " << node_value << std::endl;

        // Rmove current node from the rb-tree.
        MyRemoveFromRbTree(data, &root);

        // Check
        if (!IsEmptyRbRoot(&root) && !IsLegalRbTree(&root)) {
            return false;
        }
        datas.pop();

        if (print_log) {
            std::cout << "Successfully remove node " << node_value << std::endl;
        }
        if (!IsEmptyRbRoot(&root) && print_log) {
            MyPrintRbTree(root.rb_node);
        }
    }

    return true;
}

bool RbTreeTesterWithValues(const std::vector<int>& values, bool print_log) {

    RbRoot root = InitializedRbRoot;
    std::queue<MyData*> datas;

    // Test the insert function.
    for (int value : values) {
        MyData* my_data_struct = reinterpret_cast<MyData*>(malloc(sizeof(MyData)));
        assert(my_data_struct != nullptr);
        my_data_struct->value = value;
        // Record rb-tree node.
        datas.push(my_data_struct);
        // Insert the new node to the rb-tree.
        MyInsertIntoRbTree(my_data_struct, &root);
    }
    std::cout << "Inserted all nodes." << std::endl;

    // Check whether the rb-tree is legal.
    if (!IsEmptyRbRoot(&root) && !IsLegalRbTree(&root)) {
        return false;
    }
    std::cout << "Passed check after inserting." << std::endl;

    if (print_log) MyPrintRbTree(root.rb_node);

    // Test the remove function.
    while (!datas.empty()) {
        MyData* data = datas.front();
        int node_value = data->value;

        if (print_log) std::cout << "Removing node " << node_value << std::endl;

        // Rmove current node from the rb-tree.
        MyRemoveFromRbTree(data, &root);

        // Check
        if (!IsEmptyRbRoot(&root) && !IsLegalRbTree(&root)) {
            return false;
        }
        datas.pop();

        if (print_log) {
            std::cout << "Successfully remove node " << node_value << std::endl;
        }
        if (!IsEmptyRbRoot(&root) && print_log) {
            MyPrintRbTree(root.rb_node);
        }
    }

    return true;
}
