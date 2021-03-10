#include "structure.h"

TREE_t * TreeCreate()
{
  TREE_t * t = (TREE_t*) malloc(sizeof(TREE_t));

  if(!t)
  {
    return ERROR;
  }

  t->root = NodeCreate(0);

  if(!t->root)
  {
    free(t);
    return ERROR;
  }

  return t;
}

NODE_t * NodeCreate(int key)
{
  NODE_t * n = (NODE_t*) malloc(sizeof(NODE_t));

  if(!n)
    return ERROR;

  n->left = NULL;
  n->right = NULL;
  n->key = key;
  n->height = 0;

  return n;
}

int TreeDestroy(TREE_t * tree)
{
  if (tree == NULL)
    return ERROR;

  NodeDestroy(tree->root);

  free(tree);

  return SUCCESS;
}

int NodeDestroy(NODE_t * node)
{
  if (node != NULL)
  {
    NodeDestroy(node->left);
    NodeDestroy(node->right);
    free(node);
  }

  return SUCCESS;
}

int TreeRotate(NODE_t ** root, char side)
{
  if (root == NULL || *root == NULL)
    return ERROR;

  NODE_t * oldroot = NULL;
  NODE_t * newroot = NULL;
  NODE_t * oldmiddle = NULL;

  if (side == 'l')
  {
    oldroot = *root;
    newroot = oldroot->left;
    oldmiddle = newroot->right;

    oldroot->left = oldmiddle;
    newroot->right = oldroot;
    *root = newroot;

    NodeFixHeight((*root)->right);
    NodeFixHeight(*root);
  }
  else
  {
    oldroot = *root;
    newroot = oldroot->right;
    oldmiddle = newroot->left;

    oldroot->right = oldmiddle;
    newroot->left = oldroot;
    *root = newroot;

    NodeFixHeight((*root)->left);
    NodeFixHeight(*root);

  }

  return SUCCESS;
}

int TreeRebalance(NODE_t ** node)
{
  if (node == NULL)
    return ERROR;

  if (*node == NULL)
    return ERROR;

  if(NodeGetHeight((*node)->left) > NodeGetHeight((*node)->right) + 1)
  {
    if(NodeGetHeight((*node)->left->left) > NodeGetHeight((*node)->left->right))
    {
      TreeRotate(node, 'l');
    }
    else
    {
      TreeRotate(&((*node)->left), 'r');
      TreeRotate(node, 'l');
    }
  }

  if(NodeGetHeight((*node)->right) > NodeGetHeight((*node)->left) + 1)
  {
    if(NodeGetHeight((*node)->right->right) > NodeGetHeight((*node)->right->left))
    {
      TreeRotate(node, 'r');
    }
    else
    {
      TreeRotate(&((*node)->right), 'l');
      TreeRotate(node, 'r');
    }

    return SUCCESS;
  }

  NodeFixHeight(*node);

  return SUCCESS;
}

int NodeFixHeight(NODE_t * node)
{
  if (node == NULL)
    return ERROR;

  node->height = 1 + MAX(NodeGetHeight(node->left), NodeGetHeight(node->right));

  return SUCCESS;
}

int TreeDeleteMin(NODE_t ** node)
{
  NODE_t * oldroot = NULL;
  int minval = 0;

  if(*node == NULL)
    return ERROR;

  if ((*node)->left == NULL)
  {
    oldroot = *node;
    minval = oldroot->key;
    *node = oldroot->right;
    free(oldroot);
  }
  else
  {
    minval = TreeDeleteMin(&((*node)->left));
  }

  TreeRebalance(node);

  return minval;
}

int TreeDeleteNode(NODE_t ** node, int key)
{
  if(*node == NULL || node == NULL)
    return ERROR;

  NODE_t * oldroot = NULL;

  if ((*node)->key == key)
  {
    if((*node)->right != NULL)
    {
      (*node)->key = TreeDeleteMin(&((*node)->right));
    }
    else
    {
      oldroot = *node;
      *node = (*node)->left;
      free(oldroot);
    }
  }
  else
  {
    if (key > (*node)->key)
    {
      TreeDeleteNode(&((*node)->right), key);
    }
    else
    {
      TreeDeleteNode(&((*node)->left), key);
    }
  }

  TreeRebalance(node);

  return SUCCESS;
}

int NodeGetHeight(NODE_t * node)
{
  if (node != NULL)
    return node->height;
  else
    return 0;
}

int TreeInsert(NODE_t ** node, int key)
{
  if (node == NULL)
    return ERROR;

  if (*node == NULL)
  {
    *node = NodeCreate(key);
    (*node)->height = 1;
    return SUCCESS;
  }
  if ((*node)->key == key)
  {
    return SUCCESS;
  }
  else
  {
    if (key > (*node)->key)
    {
      TreeInsert(&((*node)->right), key);
    }
    else
    {
      TreeInsert(&((*node)->left), key);
    }

    TreeRebalance(node);

    return SUCCESS;
  }
}

NODE_t * TreeSearch(NODE_t * node, int key)
{
  if (node == NULL)
    return 0;
  else if (key == node->key)
    return node;
  else if (key > node->key)
    return TreeSearch(node->right, key);
  else
    return TreeSearch(node->left, key);
}


int TreeForEach(TREE_t * tree, void (*foo)(NODE_t * node, void * data), void * data)
{
  if (tree == NULL)
    return ERROR;

  if (foo == NULL)
    return ERROR;

  TreeDfs(tree->root, foo, data);

  return SUCCESS;
}

void TreeDfs(NODE_t * node, void (*foo)(NODE_t * node, void * data), void * data)
{
  foo(node, data);

  if(node->left)
    TreeDfs(node->left, foo, data);
  if(node->right)
    TreeDfs(node->right, foo, data);
}

void root_graph(NODE_t * n, FILE * f_dot)
{
  if (n->left != NULL)
  {
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"left\"]\n\n", n->key, n->key, n->left->key);
    root_graph(n->left, f_dot);
  }
  if (n->right != NULL)
  {
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"right\"]\n\n", n->key, n->key, n->right->key);
    root_graph(n->right, f_dot);
  }
}

void digraph(NODE_t * n, char * filename)
{
  if (n == NULL || filename == NULL)
    return;


  FILE * f_dot = fopen(filename, "w+");
  assert(f_dot);

  fprintf(f_dot, "digraph {\n");
  fprintf(f_dot, "\t\tnode [shape=\"circle\", style=\"filled\", fillcolor=\"blue\", fontcolor=\"#FFFFFF\", margin=\"0.01\"];\n");
  fprintf(f_dot, "\t\tedge [style=\"dashed\"];\n\n");

  root_graph(n, f_dot);

  fprintf(f_dot, "}");
  fclose(f_dot);

  char * comm = (char*)calloc(50, sizeof(char));
  comm[0] = 'd';
  comm[1] = 'o';
  comm[2] = 't';
  comm[3] = ' ';
  comm[4] = '-';
  comm[5] = 'T';
  comm[6] = 'j';
  comm[7] = 'p';
  comm[8] = 'g';
  comm[9] = ' ';
  strcat(comm, filename);
  comm[14] = ' ';
  comm[15] = '-';
  comm[16] = 'o';
  comm[17] = ' ';
  strcat(comm, filename);
  comm[22] = '.';
  comm[23] = 'j';
  comm[24] = 'p';
  comm[25] = 'g';

  system(comm);

  free(comm);
}
