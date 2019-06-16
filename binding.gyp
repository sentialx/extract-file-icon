{
  "targets": [
    {
      "target_name": "addon",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions" ],
      'conditions': [
        ['OS=="win"', {
          'link_settings': {
            'libraries': [
              'Gdiplus.lib',
            ],
          },
          'sources': [
            'lib/addon.cc',
          ],
        }],
        ['OS=="mac"', {
          'link_settings': {
            'libraries': [
              '-framework AppKit',
            ],
          },
          'sources': [
            'lib/addon.mm',
          ],
        }],
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")"
      ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
    }
  ]
}
