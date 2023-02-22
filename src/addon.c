#include <node_api.h>

#include "merkle.h"

#define NAPI_CALL(env, call)                                             \
  do {                                                                   \
    napi_status status = (call);                                         \
    if (status != napi_ok) {                                             \
      const napi_extended_error_info* error_info = NULL;                 \
      napi_get_last_error_info((env), &error_info);                      \
      const char* err_message = error_info->error_message;               \
      bool is_pending;                                                   \
      napi_is_exception_pending((env), &is_pending);                     \
      if (!is_pending) {                                                 \
        const char* message =                                            \
            (err_message == NULL) ? "empty error message" : err_message; \
        napi_throw_error((env), NULL, message);                          \
        return NULL;                                                     \
      }                                                                  \
    }                                                                    \
  } while (0)

napi_value create_hash_string(napi_env env, unsigned char hash[32]) {
  char fmthash[67] = "0x";
  char* ptr = fmthash + 2;
  napi_value napi_hash;

  for (int i = 0; i < HASH_LEN; i++, ptr += 2) sprintf(ptr, "%02x", hash[i]);
  NAPI_CALL(env, napi_create_string_utf8(env, fmthash, 66, &napi_hash));
  return napi_hash;
}

void finalize(napi_env env, void* finalize_data, void* finalize_hint) {
  merkle_free_tree((merkle_tree_t*)finalize_hint);
}

napi_value root(napi_env env, napi_callback_info info) {
  napi_value js_obj;
  NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &js_obj, NULL));

  merkle_tree_t* tree;
  NAPI_CALL(env, napi_unwrap(env, js_obj, (void**)&tree));

  napi_value root = create_hash_string(env, tree->root->hash);
  return root;
}

napi_value proof(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_value js_obj;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &js_obj, NULL));

  if (argc != 1) {
    NAPI_CALL(env, napi_throw_type_error(env, NULL, "Number expected"));
    return NULL;
  }

  int id;
  NAPI_CALL(env, napi_get_value_int32(env, argv[0], &id));

  merkle_tree_t* tree;
  NAPI_CALL(env, napi_unwrap(env, js_obj, (void**)&tree));

  merkle_node_t* proof[MAX_PROOF_LEN];
  unsigned int prooflen = 0;
  merkle_get_proof(tree, id, proof, &prooflen);

  napi_value jsproof;
  NAPI_CALL(env, napi_create_array_with_length(env, prooflen, &jsproof));

  for (unsigned int i = 0; i < prooflen; i++) {
    napi_value subproof = create_hash_string(env, proof[i]->hash);
    NAPI_CALL(env, napi_set_element(env, jsproof, i, subproof));
  }

  return jsproof;
}

napi_value new(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value argv[1];
  napi_value js_obj;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &js_obj, NULL));

  bool is_array;
  NAPI_CALL(env, napi_is_array(env, argv[0], &is_array));
  if (argc != 1 || !is_array) {
    NAPI_CALL(env, napi_throw_type_error(env, NULL, "Array expected"));
    return NULL;
  }

  uint32_t idslen;
  NAPI_CALL(env, napi_get_array_length(env, argv[0], &idslen));
  if (idslen < 1) {
    NAPI_CALL(env, napi_throw_type_error(env, NULL, "Invalid array length"));
    return NULL;
  }

  uint16_t* ids = malloc(idslen * sizeof(uint16_t));
  for (uint32_t i = 0; i < idslen; i++) {
    napi_value val;
    NAPI_CALL(env, napi_get_element(env, argv[0], i, &val));
    NAPI_CALL(env, napi_get_value_int32(env, val, (int32_t*)&ids[i]));
    if (ids[i] < 0 || ids[i] > 9999) {
      NAPI_CALL(env, napi_throw_type_error(env, NULL, "Invalid id"));
      return NULL;
    }
  }
  merkle_tree_t* tree = merkle_new_tree(ids, idslen);
  free(ids);

  NAPI_CALL(env, napi_wrap(env, js_obj, tree, &finalize, tree, NULL));
  return NULL;
}

NAPI_MODULE_INIT() {
  merkle_init();

  const napi_property_descriptor props[] = {
      {"root", NULL, NULL, &root, NULL, NULL, napi_default, NULL},
      {"proof", NULL, &proof, NULL, NULL, NULL, napi_default, NULL},
  };

  napi_value cl;
  NAPI_CALL(
      env, napi_define_class(env, "MerkleTree", 10, &new, NULL, 2, props, &cl));
  return cl;
}