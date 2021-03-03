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
typedef struct Tree TREE_t;
typedef struct Node NODE_t;

struct Tree
{
  NODE_t * root;
};

struct Node
{
  NODE_t * left;
  NODE_t * right;
  int key;
  int height;
};


// Prototypes
//! Creates a tree witn root node key 0
TREE_t * TreeCreate();
//! Created node with key taken from parameter
NODE_t * NodeCreate(int key);
//! Searches elem with key from parameter
//! Return node with key from parameter
//! If there is no such node returns 0
NODE_t * TreeSearch(NODE_t * node, int key);
//!  Destroys tree
//! Returns ERROR if tree doenst exist
int TreeDestroy(TREE_t * tree);
//! Destroys node
//! Returns SUCCESS all time
int NodeDestroy(NODE_t * node);
//! Rotating node with selected side
//! Return ERROR if there is no such node
int TreeRotate(NODE_t ** root, char side);
//! Rebalancing all leafes of node from parameter
//! Returns error it node parameter doesnt exist
int TreeRebalance(NODE_t ** node);
//! Fix heights for tree with root from parameter
//! Returns ERROR if node doesnt exist
int NodeFixHeight(NODE_t * node);
//! Deletes leave with minimal key from tree
//! Returns ERROR if node doesnt exist
int TreeDeleteMin(NODE_t ** node);
//! Deletes leave with selected key from tree
//! Returns ERROR if node doesnt exist
int TreeDeleteNode(NODE_t ** node, int key);
//! Returns height of selected node
//! Returns 0 if node doesnt exist
int NodeGetHeight(NODE_t * node);
//! Insert elem to the tree with selected key
//! Returns only SUCCESS
int TreeInsert(NODE_t ** node, int key);
//! Iterator through tree
//! Returns ERROR if there is no tree of foo
int TreeForEach(TREE_t * tree, void (*foo)(NODE_t * node, void * data), void * data);
//! Deep field search through tree
void TreeDfs(NODE_t * node, void (*foo)(NODE_t * node, void * data), void * data);
//! Draws tree into pic.jpg
void root_graph(NODE_t * n, FILE * f_dot);
void digraph(NODE_t * n, char * filename);
