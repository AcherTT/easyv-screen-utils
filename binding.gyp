{
  'targets': [
    {
      'target_name': 'screen_utils_native',
      'sources': [ 
        'src/screen_utils.cc',
        'src/screen_importer.cc',
        'src/db_pool/mysql_pool.cc',
        'src/db_pool/pgsql_pool.cc',
        'src/main.cc'
      ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include",
      ],
      'libraries': [
        '-lpq',
        '-lzip',
        '-lmysqlclient',
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")",
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ["OS == 'mac'", {
          "xcode_settings": {
            "OTHER_CFLAGS": [
              "-mmacosx-version-min=10.7"
            ],
            "OTHER_CPLUSPLUSFLAGS": [
              "-mmacosx-version-min=10.7"
            ]
          }
        }],
        ["OS == 'linux'", {
          "cflags_cc": [
            "-std=c++17"
          ]
        }]
      ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
        "OTHER_CPLUSPLUSFLAGS": [
          "-std=c++17",
          "-stdlib=libc++"
        ]
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}