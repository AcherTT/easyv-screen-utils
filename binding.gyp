{
  'targets': [
    {
      'target_name': 'screen_utils_native',
      'sources': [ 
        'src/screen_utils.cc',
        'src/screen_importer.cc',
        'src/db_pool.cc',
        'src/main.cc'
      ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
        "/usr/include/postgresql",
      ],
      'libraries': [
        "-L/usr/lib/x86_64-linux-gnu -lpq"
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")",
      ],
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'xcode_settings': {
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'CLANG_CXX_LIBRARY': 'libc++',
        'MACOSX_DEPLOYMENT_TARGET': '10.7'
      },
      'msvs_settings': {
        'VCCLCompilerTool': { 'ExceptionHandling': 1 },
      }
    }
  ]
}