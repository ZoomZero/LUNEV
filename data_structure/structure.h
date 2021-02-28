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

typedef struct Iterator
{


} ITER;

TREE * TreeCreate();
NODE * NodeCreate(int key);
int TreeDestroy(TREE * tree);
int NodeDestroy(NODE * node);
int TreeRotate(NODE * root, char side);
int TreeRebalance(NODE * node);
int NodeFixHeight(NODE * node);
int TreeDeleteMin(NODE * node);
int TreeDeleteNode(NODE * node, int key);
int NodeGetHeight(NODE * node);
NODE * TreeInsert(NODE * node, int key);
NODE * TreeSearch(TREE tree, int key);
void root_graph(NODE * n, FILE * f_dot);
void digraph(NODE * n, char * filename);
