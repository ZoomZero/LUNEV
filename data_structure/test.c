#include "structure.h"

#define MULTI 6

void cons(NODE * node, void * data)
{

}

void test(TREE * tree)
{
  for (int i = 0; i < 10; i++)
  {
    TreeInsert(&(tree->root), (i*MULTI+1) % 10);
  }

  TreeDeleteMin(&(tree->root));

  TreeDeleteNode(&(tree->root), 7);
  TreeDeleteNode(&(tree->root), 11);

  TreeSearch(tree->root, 1);

  TreeForEach(tree, cons, NULL);
}

int main(int argc, char const *argv[])
{
  TREE * tree;
  tree = TreeCreate();

  char pic1[5] = "pic1";
  digraph(tree->root, pic1);

  test(tree);

  char pic3[5] = "pic3";
  digraph(tree->root, pic3);

  TreeDestroy(tree);

  return 0;
}
