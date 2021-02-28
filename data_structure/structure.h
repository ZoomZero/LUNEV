#include <stdlib.h>
#include <stdio.h>

#define SUCCESS 1
#define ERROR -1

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
int TreeRotate(TREE * tree, NODE * root, char side);
int TreeRebalance(TREE * tree);
int TreeFixHeight(TREE * tree);
int * TreeDeleteNode(TREE * tree, int key);
int TreeGetHeight(TREE tree);
int TreeInsert(TREE * tree, int key);
NODE * TreeSearch(TREE tree, int key);
