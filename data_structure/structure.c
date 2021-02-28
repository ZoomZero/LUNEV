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

int TreeRotate(NODE * root, char side)
{
  NODE * oldroot;
  NODE * newroot;
  NODE * oldmiddle;

  if (side == 'l')
  {
    oldroot = root;
    newroot = oldroot->left;
    oldmiddle = newroot->right;

    oldroot->left = oldmiddle;
    newroot->right = oldroot;
    root = newroot;

    NodeFixHeight(root->right);
    NodeFixHeight(root);
  }
  else
  {
    oldroot = root;
    newroot = oldroot->right;
    oldmiddle = newroot->left;

    oldroot->right = oldmiddle;
    newroot->left = oldroot;
    root = newroot;

    NodeFixHeight(root->left);
    NodeFixHeight(root);
  }

  return SUCCESS;
}

int TreeRebalance(NODE * node)
{
  if (node != NULL)
  {
    if(NodeGetHeight(node->left) > NodeGetHeight(node->right) + 1)
    {
      if(NodeGetHeight(node->left->left) > NodeGetHeight(node->left->right))
      {
        TreeRotate(node, 'l');
      }
      else
      {
        TreeRotate(node->left, 'r');
        TreeRotate(node, 'l');
      }
    }
    else
    {
      if(NodeGetHeight(node->right->right) > NodeGetHeight(node->right->left))
      {
        TreeRotate(node, 'r');
      }
      else
      {
        TreeRotate(node->left, 'l');
        TreeRotate(node, 'r');
      }
    }
  }
  else
    return ERROR;

  return SUCCESS;
}

int NodeFixHeight(NODE * node)
{
  if (node == NULL)
    return ERROR;

  node->height = 1 + MAX(NodeGetHeight(node->left), NodeGetHeight(node->right));

  return SUCCESS;
}

int TreeDeleteMin(NODE * node)
{
  NODE * oldroot;
  int minval;

  if(node == NULL)
    return ERROR;

  if (node->left == NULL)
  {
    oldroot = node;
    minval = oldroot->key;
    node = oldroot->right;
    free(oldroot);
  }
  else
  {
    minval = TreeDeleteMin(node->left);
  }

  TreeRebalance(node);

  return minval;
}

int TreeDeleteNode(NODE * node, int key)
{
  NODE * oldroot;

  if(node == NULL)
    return ERROR;
  else if (node->key == key)
  {
    if(node->right != NULL)
    {
      node->key = TreeDeleteMin(node->right);
    }
    else
    {
      oldroot = node;
      node = node->left;
      free(oldroot);
    }
  }
  else
  {
    if (key > node->key)
    {
      TreeDeleteNode(node->right, key);
    }
    else
    {
      TreeDeleteNode(node->left, key);
    }
  }

  TreeRebalance(node);

  return SUCCESS;
}

int NodeGetHeight(NODE * node)
{
  if (node != NULL)
    return node->height;
  else
    return 0;
}

int TreeInsert(NODE * node, int key)
{
  if (node == NULL)
  {
    node = NodeCreate(key);
  }
  else if (node->key == key)
  {
    return SUCCESS;
  }
  else
  {
    if (key > node->key)
    {
      TreeInsert(node->right, key);
    }
    else
    {
      TreeInsert(node->left, key);
    }

    TreeRebalance(node);
  }

  return SUCCESS;
}

NODE * TreeSearch(TREE tree, int key)
{
  NODE * n = NULL;

  return n;
}

void root_graph(NODE * n, FILE * f_dot)
{
  if (n->left != NULL)
  {
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"Yes\"]\n\n", n->height, n->height, n->left->height);
    root_graph(n->left, f_dot);
  }
  if (n->right != NULL)
  {
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"No\"]\n\n", n->height, n->height, n->right->height);
    root_graph(n->right, f_dot);
  }
}

void digraph(NODE * n, char * filename)
{
  assert(n);
  assert(filename);

  FILE * f_dot = fopen(filename, "w+");
  assert(f_dot);

  fprintf(f_dot, "digraph {\n");
  fprintf(f_dot, "\t\tnode [shape=\"circle\", style=\"filled\", fillcolor=\"blue\", fontcolor=\"#FFFFFF\", margin=\"0.01\"];\n");
  fprintf(f_dot, "\t\tedge [style=\"dashed\"];\n\n");

  root_graph(n, f_dot);

  fprintf(f_dot, "}");
  fclose(f_dot);

  system("dot -Tjpg digraph -o tree.jpg");
}

int main(int argc, char const *argv[])
{
  
  return 0;
}
