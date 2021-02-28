#include "structure.h"

TREE * TreeCreate()
{
  TREE * t = (TREE*) malloc(sizeof(TREE));

  if(!t)
    return ERROR;

  t->root = NodeCreate(666);

  if(!t->root)
  {
    free(t);
    return ERROR;
  }

  return t;
}

NODE * NodeCreate(int key)
{
  NODE * n = (NODE*) malloc(sizeof(NODE));

  if(!n)
    return ERROR;

  n->left = NULL;
  n->right = NULL;
  n->key = key;
  n->height = 0;

  return n;
}

int TreeDestroy(TREE * tree)
{
  if (tree == NULL) return ERROR;

  NodeDestroy(tree->root);

  return SUCCESS;
}

int NodeDestroy(NODE * node)
{
  if (node != NULL)
  {
    NodeDestroy(node->left);
    NodeDestroy(node->right);
    free(node);
  }

  return SUCCESS;
}

int TreeRotate(TREE * tree, NODE * root, char side)
{

  return SUCCESS;
}

int TreeRebalance(TREE * tree)
{

  return SUCCESS;
}

int TreeFixHeight(TREE * tree)
{

  return SUCCESS;
}

int * TreeDeleteNode(TREE * tree, int key)
{

  return SUCCESS;
}

int TreeGetHeight(TREE tree)
{
  if (tree->root != NULL)
  {
    return tree->root->
  }
  return SUCCESS;
}

int TreeInsert(TREE * tree, int key)
{

  return SUCCESS;
}

NODE * TreeSearch(TREE tree, int key)
{
  NODE * n = NULL;

  return n;
}
