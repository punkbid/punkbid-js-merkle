{
  "targets": [
    {
      "target_name": "merkle",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      "sources": ["src/keccak256.c", "src/addon.c", "src/merkle.c"],
      "include_dirs": ["<!(node -p \"require('node-addon-api').include_dir\")"],
      "defines": ["NAPI_DISABLE_CPP_EXCEPTIONS"],
      "conditions": [
        [
          "OS==\"mac\"",
          {
            "cflags+": ["-fvisibility=hidden"],
            "xcode_settings": {
              "GCC_SYMBOLS_PRIVATE_EXTERN": "YES"
            }
          }
        ]
      ]
    }
  ]
}
