#include "merkle.h"

unsigned char leaves[COLLECTION_LEN][HASH_LEN];

void merkle_init() {
  SHA3_CTX ctx;
  unsigned char msg[HASH_LEN];

  for (uint16_t id = 0; id < 10000; id++) {
    uint8_t* idb = (uint8_t*)&id;

    memset(msg, 0, sizeof(msg));
    memcpy(msg + 31, &idb[0], 1);
    memcpy(msg + 30, &idb[1], 1);

    keccak_init(&ctx);
    keccak_update(&ctx, msg, sizeof(msg));
    keccak_final(&ctx, leaves[id]);
  }
}

unsigned int merkle_treelen(unsigned int levellen) {
  unsigned int treelen = levellen;

  while (levellen > 1) {
    levellen = ceil(levellen / 2.0);
    treelen += levellen;
  }
  return treelen;
}

int merkle_nodecmp(const void* a, const void* b) {
  return memcmp(a, b, HASH_LEN);
}

void merkle_compute_tree(merkle_tree_t* tree) {
  merkle_node_t* node = tree->nodes + tree->idslen;
  merkle_node_t* level = tree->nodes;
  int levellen = tree->idslen;

  while (levellen > 1) {
    for (int i = 0; i < levellen; i += 2) {
      merkle_node_t* pair[2] = {&level[i]};
      SHA3_CTX ctx;

      if (i + 1 == levellen) {
        if (levellen % 2 == 1) {
          *node = level[i];
          node++;
          continue;
        }
        pair[1] = &level[i];
      } else {
        pair[1] = &level[i + 1];
      }

      keccak_init(&ctx);
      int res = merkle_nodecmp(pair[0]->hash, pair[1]->hash);
      keccak_update(&ctx, pair[res > 0 ? 1 : 0]->hash, HASH_LEN);
      keccak_update(&ctx, pair[res > 0 ? 0 : 1]->hash, HASH_LEN);
      keccak_final(&ctx, node->hash);
      node++;
    }

    level += levellen;
    levellen = ceil(levellen / 2.0);
  }
  tree->root = node - 1;
}

merkle_tree_t* merkle_new_tree(uint16_t ids[], int idslen) {
  merkle_tree_t* tree = malloc(sizeof(merkle_tree_t));
  int treelen = merkle_treelen(idslen);

  if (tree == NULL) return NULL;
  if ((tree->nodes = malloc(treelen * sizeof(merkle_node_t))) == NULL)
    return NULL;
  tree->idslen = idslen;

  for (unsigned int i = 0; i < tree->idslen; i++)
    memcpy(tree->nodes + i, leaves[ids[i]], HASH_LEN);
  qsort(tree->nodes, idslen, sizeof(merkle_node_t), merkle_nodecmp);
  merkle_compute_tree(tree);
  return tree;
}

void merkle_free_tree(merkle_tree_t* tree) {
  free(tree->nodes);
  free(tree);
}

int merkle_find_index(merkle_tree_t* tree, uint16_t id) {
  for (unsigned int i = 0; i < tree->idslen; i++) {
    if (memcmp(tree->nodes[i].hash, leaves[id], HASH_LEN) == 0) return i;
  }
  return -1;
}

void merkle_get_proof(merkle_tree_t* tree, uint16_t id, merkle_node_t** proof,
                      unsigned int* prooflen) {
  int index = merkle_find_index(tree, id);
  *prooflen = 0;
  if (index == -1) return;

  merkle_node_t* level = tree->nodes;
  int levellen = tree->idslen;
  merkle_node_t** ptpr = proof;

  while (levellen > 1) {
    unsigned short is_right = index % 2;
    int pair_index = -1;

    if (is_right == 1) {
      pair_index = index - 1;
    } else if (index + 1 < levellen) {
      pair_index = index + 1;
    }

    if (pair_index != -1) {
      *ptpr = level + pair_index;
      ptpr++;
      *prooflen += 1;
    }

    level += levellen;
    levellen = ceil(levellen / 2.0);
    index = index / 2;
  }
}
