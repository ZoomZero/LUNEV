#include "structure.h"

TREE * TreeCreate()
{
  TREE * t = (TREE*) malloc(sizeof(TREE));

  if(!t)
    return ERROR;

  t->root = NodeCreate(0);

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

  printf("created node with key = %d\n", key);

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

int TreeRotate(NODE ** root, char side)
{
  NODE * oldroot;
  NODE * newroot;
  NODE * oldmiddle;

  if (side == 'l')
  {
    printf("doing rotating l \n");
    oldroot = *root;
    newroot = oldroot->left;
    oldmiddle = newroot->right;

    printf("did smthing\n");

    oldroot->left = oldmiddle;
    newroot->right = oldroot;
    *root = newroot;

    NodeFixHeight((*root)->right);
    NodeFixHeight(*root);
  }
  else
  {
    printf("doing rotating r\n");
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

int TreeRebalance(NODE ** node)
{
  if (node != NULL)
  {
    printf("height of chld %d %d\n", NodeGetHeight((*node)->left), NodeGetHeight((*node)->right));

    if(NodeGetHeight((*node)->left) > NodeGetHeight((*node)->right) + 1)
    {
      printf("rebalancing left\n");
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
      printf("rebalancing right\n");
      printf("nr k = %d\n", (*node)->right->key);
      printf("height of chld chld %d %d\n", NodeGetHeight((*node)->right->right), NodeGetHeight((*node)->right->left));
      if(NodeGetHeight((*node)->right->right) > NodeGetHeight((*node)->right->left))
      {
        printf("rotating right in rebalancing\n");
        TreeRotate(node, 'r');
      }
      else
      {
        printf("rotating left in rebalancing\n");
        TreeRotate(&((*node)->right), 'l');
        TreeRotate(node, 'r');
        printf("finished left rotating in rebalancing\n");
      }

      return SUCCESS;
    }

    NodeFixHeight(*node);

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

  TreeRebalance(&node);

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

  TreeRebalance(&node);

  return SUCCESS;
}

int NodeGetHeight(NODE * node)
{
  if (node != NULL)
    return node->height;
  else
    return 0;
}

int TreeInsert(NODE ** node, int key)
{
  if (*node == NULL)
  {
    printf("node null in insert\n");
    *node = NodeCreate(key);
    (*node)->height = 1;
    printf("returning with success\n");
    return SUCCESS;
  }
  else if ((*node)->key == key)
  {
    printf("success in insert\n");
    return SUCCESS;
  }
  else
  {
    if (key > (*node)->key)
    {
      printf("inserting in right(parent with key = %d)\n", (*node)->key);
      TreeInsert(&((*node)->right), key);
    }
    else
    {
      printf("inserting in left\n");
      TreeInsert(&((*node)->left), key);
    }

    printf("starting rebalancing in insert\n");
    TreeRebalance(node);

    return SUCCESS;
  }
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
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"left\"]\n\n", n->key, n->key, n->left->key);
    root_graph(n->left, f_dot);
  }
  if (n->right != NULL)
  {
    fprintf(f_dot, "\t\"%d\"\n\t\t\"%d\"->\"%d\" [label = \"right\"]\n\n", n->key, n->key, n->right->key);
    root_graph(n->right, f_dot);
  }
  if (n->right == NULL && n->left == NULL)
  {
        fprintf(f_dot, "\t\"%d\"\n", n->height);
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

  //printf("%s\n", comm);

  system(comm);
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
    int key = (i*MULTI+1) % 10;
    printf("key = %d in iter = %d\n", key, i);
    TreeInsert(&(tree->root), (i*MULTI+1) % 10);
    printf("finished inserting\n");
  }

  char pic2[5] = "pic2";
  digraph(tree->root, pic2);

  return 0;
}
