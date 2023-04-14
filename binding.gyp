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
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")",
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'conditions': [
        ["OS == 'mac'", {
          'include_dirs': [
            '/opt/homebrew/opt/libpq/include',
            "/opt/homebrew/opt/mysql-client/include",
            "/opt/homebrew/Cellar/libzip/1.9.2/include",
          ],
          'libraries': [
            '-L/opt/homebrew/opt/libpq/lib',
            '-lpq',
            '-L/opt/homebrew/Cellar/libzip/1.9.2/lib',
            '-lzip',
            '-L/opt/homebrew/opt/mysql-client/lib'
            '-lmysqlclient',
          ],
          "xcode_settings": {
            "OTHER_CFLAGS": [
              "-mmacosx-version-min=10.7"
            ],
            "OTHER_CPLUSPLUSFLAGS": [
              "-mmacosx-version-min=10.7"
            ],
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'CLANG_CXX_LIBRARY': 'libc++',
            'MACOSX_DEPLOYMENT_TARGET': '10.7',
            "OTHER_CPLUSPLUSFLAGS": [
              "-std=c++17",
              "-stdlib=libc++"
            ]
          }
        }],
        ["OS == 'linux'", {
          'include_dirs': [
            "/usr/include/postgresql",
            "/usr/include",
          ],
          'libraries': [
            '-lpq',
            '-lzip',
            '-lmysqlclient',
          ],
          "cflags_cc": [
            "-std=c++17"
          ],
        }]
      ],
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}