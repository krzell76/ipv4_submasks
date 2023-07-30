#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MASK_MAX 32
#define NO_PREFIX 0
#define IS_ERROR -1
#define IS_OK 0

struct TrieNode {
  struct TrieNode *zero;
  struct TrieNode *one;
  char mask;
};

struct TrieNode MainRoot = {
  .zero = NULL, 
  .one = NULL,
  .mask = NO_PREFIX
};

struct TrieNode *create_node(void) {
  struct TrieNode *new_node = (struct TrieNode *)malloc(sizeof(struct TrieNode));
  if (new_node != NULL) {
    new_node->zero = NULL;
    new_node->one = NULL;
    new_node->mask = NO_PREFIX;
  }
  return new_node;
}

// returns true if ok
bool check_input(unsigned int base, char mask) {
  unsigned int full_mask = 0xFFFFFFFF << (MASK_MAX - mask);
  unsigned int masked_base = base & full_mask;

  if (mask < 0 || mask > MASK_MAX)
    return false;

  return masked_base == base;
}

int add_helper(struct TrieNode *root, unsigned int base, char mask) {
  struct TrieNode *temp = root;

  //printf("add helper b %x, m %d\n", base, mask);
  for (int i = MASK_MAX - 1; i >= MASK_MAX - mask; i--) {
    unsigned int bit = (base >> i) & 1;

    if (bit == 0) {
      if (!temp->zero)
        temp->zero = create_node();
      temp = temp->zero;
    } 
    else {
      if (!temp->one)
        temp->one = create_node();
      temp = temp->one;
    }
  }
  if (temp == NULL)
    return IS_ERROR;
  temp->mask = mask;
  return IS_OK;
}

int add(unsigned int base, char mask) {
  printf("\tadd %x %d\n", base, mask);
  if (!check_input(base, mask))
    return IS_ERROR;
  return add_helper(&MainRoot, base, mask);
}


int del_helper(struct TrieNode *root, unsigned int base, char mask) {
  //printf("del helper b %x, m %u\n", base, mask);
  if (root == NULL)
      return IS_ERROR;

  if (mask == NO_PREFIX) {
    //printf("del helper root->mask %d\n", root->mask);
    if (root->mask != NO_PREFIX){
      root->mask = NO_PREFIX;
      return IS_OK;
    }
  } 
  else {
      unsigned int bit = (base >> (MASK_MAX - 1)) & 1;
      //printf("del helper bit %d\n", bit);
      if (bit == 0) {
          if (del_helper(root->zero, base<<1, mask -1) == IS_OK) {
              if (root->zero->zero == NULL && root->zero->one == NULL) {
                  free(root->zero);
                  root->zero = NULL;
              }
              return IS_OK;
          }
      } 
      else {
          if (del_helper(root->one, base <<1, mask -1) == IS_OK) {
              if (root->one->zero == NULL && root->one->one == NULL) {
                  free(root->one);
                  root->one = NULL;
              }
              return IS_OK;
          }
      }
  }

  return IS_ERROR;
}

int del(unsigned int base, char mask) {
  printf("\tdel %x, %d\n", base, mask);
  return del_helper(&MainRoot, base, mask);
}

int check_helper(struct TrieNode *root, unsigned int ip) {
  //printf("check_helper ip %x\n", ip);
  struct TrieNode* temp = root;
  if (root == NULL)
    return IS_ERROR;

  for (int i = MASK_MAX - 1; i >= 0; --i) {
    unsigned int bit = (ip >> i) & 1;
    //printf("check_helper bit %x\n", bit);
    if (bit == 1){
      temp = temp->one;
    }
    else
      temp = temp->zero;
    if (temp == NULL)
      break;
    if (temp->mask != NO_PREFIX)
      return temp->mask;
  }
  return IS_ERROR;
}


int check(unsigned int ip) { 
  printf("\tcheck\n");
  return check_helper(&MainRoot, ip); 
}

void dump_helper(struct TrieNode *root, int sub_mask) {
  if (root == NULL) {
    return;
  }
  dump_helper(root->zero, (sub_mask << 1));
  dump_helper(root->one, (sub_mask << 1) | 1);

  if (root->mask != NO_PREFIX) {
    unsigned int mask = sub_mask << (32 - root->mask);
    printf("%u.%u.%u.%u/%d\n", (mask >> 24) & 0xFF, (mask >> 16) & 0xFF,
           (mask >> 8) & 0xFF, mask & 0xFF, root->mask);
  }
}

void dump(void) {
  printf("\tdump\n");
  dump_helper(&MainRoot, 0); 
}
