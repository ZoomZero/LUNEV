#include "structure.h"

#define MULTI 7

void cons(NODE * node, void * data)
{

}

void test1(TREE * tree)
{
  for (int i = 0; i < 20; i++)
  {
    TreeInsert(&(tree->root), (i*MULTI+1) % 10);
  }

  TreeDeleteMin(&(tree->root));

  for(int i = 0; i < 10; i++)
  {
    TreeSearch(tree->root, i);
  }

  for(int i = 1; i < 10; i = i + 2)
  {
    TreeDeleteNode(&(tree->root), i);
  }

  for (int i = 0; i < 10; i++)
  {
    TreeInsert(&(tree->root), (-i*MULTI+1) % 10);
  }

  TreeForEach(tree, cons, NULL);
}

void test2(TREE * tree)
{
  TreeDestroy(tree);
  TreeForEach(tree, cons, NULL);

  tree = TreeCreate();
  free(tree->root);
  tree->root = NULL;

  TreeDeleteMin(&(tree->root));
  TreeDeleteNode(&(tree->root), 0);
  NodeFixHeight(tree->root);
  TreeRebalance(&(tree->root));
  TreeForEach(tree, NULL, NULL);

  TreeDestroy(tree);
}

int main(int argc, char const *argv[])
{
  TREE * tree;
  tree = TreeCreate();

  TREE * tree2 = NULL;

  char pic1[5] = "pic1";
  digraph(tree->root, pic1);

  test1(tree);
  test2(tree2);

  char pic3[5] = "pic3";
  digraph(tree->root, pic3);

  TreeDestroy(tree);



  return 0;
}
