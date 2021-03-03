#include "structure.h"

#define MULTI 7
#define NUMBER 20
#define NUMBER2 10

void cons(NODE_t * node, void * data)
{

}

void test1(TREE_t * tree)
{
  for (int i = 0; i < NUMBER; i++)
  {
    TreeInsert(&(tree->root), (i*MULTI+1) % 10);
  }

  TreeDeleteMin(&(tree->root));

  for(int i = 0; i < NUMBER2; i++)
  {
    TreeSearch(tree->root, i);
  }

  for(int i = 1; i < NUMBER2; i = i + 2)
  {
    TreeDeleteNode(&(tree->root), i);
  }

  for (int i = 0; i < NUMBER2; i++)
  {
    TreeInsert(&(tree->root), (-i*MULTI+1) % 10);
  }

  TreeForEach(tree, cons, NULL);
}

void test2(TREE_t * tree)
{
  TreeDestroy(tree);
  TreeForEach(tree, cons, NULL);

  tree = TreeCreate();
  free(tree->root);
  tree->root = NULL;

  TreeDeleteMin(&(tree->root));
  TreeDeleteNode(&(tree->root), 0);
  TreeInsert(NULL, 0);
  NodeFixHeight(tree->root);
  TreeRebalance(&(tree->root));
  TreeForEach(tree, NULL, NULL);

  char pic3[5] = "pic3";
  digraph(tree->root, pic3);

  TreeDestroy(tree);
}

int main(int argc, char const *argv[])
{
  TREE_t * tree;
  tree = TreeCreate();

  TREE_t * tree2 = NULL;

  char pic1[5] = "pic1";
  digraph(tree->root, pic1);

  test1(tree);
  test2(tree2);

  char pic2[5] = "pic2";
  digraph(tree->root, pic2);

  TreeDestroy(tree);

  return 0;
}
