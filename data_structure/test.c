#include "structure.h"

void test()
{

}

int main(int argc, char const *argv[])
{
  TREE * tree;
  tree = TreeCreate();

  char pic1[5] = "pic1";
  digraph(tree->root, pic1);

  int MULTI = 6;

  for (int i = 0; i < 10; i++)
  {
    TreeInsert(&(tree->root), (i*MULTI+1) % 10);
  }

  char pic2[5] = "pic2";
  digraph(tree->root, pic2);

  TreeDeleteMin(&(tree->root));

  TreeDeleteNode(&(tree->root), 7);

  char pic3[5] = "pic3";
  digraph(tree->root, pic3);

  TreeDestroy(tree);

  return 0;
}
