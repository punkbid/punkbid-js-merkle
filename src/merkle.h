#ifndef _MERKLE_H_
#define _MERKLE_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keccak256.h"

#define COLLECTION_LEN 10000

#define HASH_LEN 32

#define MAX_PROOF_LEN 14

typedef struct {
  unsigned char hash[HASH_LEN];
} merkle_node_t;

typedef struct {
  unsigned int idslen;
  merkle_node_t* nodes;
  merkle_node_t* root;
} merkle_tree_t;

void merkle_init();
merkle_tree_t* merkle_new_tree(uint16_t ids[], int idslen);
void merkle_free_tree(merkle_tree_t* tree);
void merkle_get_proof(merkle_tree_t* tree, uint16_t id, merkle_node_t** proof,
                      unsigned int* prooflen);

#endif  // _MERKLE_H_