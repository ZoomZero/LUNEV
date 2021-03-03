#pragma once

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Constants
#define SUCCESS 1
#define ERROR -1

#define MAX(x, y) ((x) > (y) ? (x) : (y))

// Declaration
typedef struct Tree TREE;
typedef struct Node NODE;

// Prototypes
//! Creates a tree witn root node key 0
TREE * TreeCreate();
//! Created node with key taken from parameter
NODE * NodeCreate(int key);
//! Searches elem with key from parameter
//! Return node with key from parameter
//! If there is no such node returns 0
NODE * TreeSearch(NODE * node, int key);
//!  Destroys tree
//! Returns ERROR if tree doenst exist
int TreeDestroy(TREE * tree);
//! Destroys node
//! Returns SUCCESS all time
int NodeDestroy(NODE * node);
//! Rotating node with selected side
//! Return SUCCESS
int TreeRotate(NODE ** root, char side);
//! Rebalancing all leafes of node from parameter
//! Returns error it node parameter doesnt exist
int TreeRebalance(NODE ** node);
//! Fix heights for tree with root from parameter
//! Returns ERROR if node doesnt exist
int NodeFixHeight(NODE * node);
//! Deletes leave with minimal key from tree
//! Returns ERROR if node doesnt exist
int TreeDeleteMin(NODE ** node);
//! Deletes leave with selected key from tree
//! Returns ERROR if node doesnt exist
int TreeDeleteNode(NODE ** node, int key);
//! Returns height of selected node
//! Returns 0 if node doesnt exist
int NodeGetHeight(NODE * node);
//! Insert elem to the tree with selected key
//! Returns only SUCCESS
int TreeInsert(NODE ** node, int key);
//! Iterator through tree
//! Returns ERROR if there is no tree of foo
int TreeForEach(TREE * tree, void (*foo)(NODE * node, void * data), void * data);
//! Deep field search through tree
void TreeDfs(NODE * node, void (*foo)(NODE * node, void * data), void * data);
//! Draws tree into pic.jpg
void root_graph(NODE * n, FILE * f_dot);
void digraph(NODE * n, char * filename);
