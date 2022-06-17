{
  'targets': [
    {
      'target_name': 'serial_addon-native',
      'sources': [ 'src/serial_addon.cc' ],
      'libraries': [ 
      		'/home/luca/Workspace/Node/Node-API/serial_addon/src/portal.o', 
      		'/home/luca/Workspace/Node/Node-API/serial_addon/src/serial.o'
      ],
      'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")"],
      'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
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
