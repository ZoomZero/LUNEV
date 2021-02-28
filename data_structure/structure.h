#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define SUCCESS 1
#define ERROR -1

#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct Node NODE;

struct Node
{
  NODE * left;
  NODE * right;
  int key;
  int height;
};

typedef struct Tree
{
  NODE * root;
} TREE;

TREE * TreeCreate();
NODE * NodeCreate(int key);
int TreeDestroy(TREE * tree);
int NodeDestroy(NODE * node);
int TreeRotate(NODE ** root, char side);
int TreeRebalance(NODE ** node);
int NodeFixHeight(NODE * node);
int TreeDeleteMin(NODE ** node);
int TreeDeleteNode(NODE ** node, int key);
int NodeGetHeight(NODE * node);
int TreeInsert(NODE ** node, int key);
int TreeForEach(TREE * tree, void (*foo)(int key, void * data), void * data);
void TreeDfs(NODE * node, void (*foo)(int key, void * data), void * data);
void root_graph(NODE * n, FILE * f_dot);
void digraph(NODE * n, char * filename);
