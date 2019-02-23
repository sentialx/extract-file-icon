{
  "targets": [
    {
      "target_name": "addon",
      "sources": [ "lib/addon.cpp" ],
      "include_dirs": ["<!(node -e \"require('nan')\")"],
      'conditions': [
        ['OS=="win"', {
          'link_settings': {
            'libraries': [
              'Gdiplus.lib',
            ],
          },
          'sources': [
            'lib/addon.cpp',
          ],
        }],
      ],
    }
  ]
}
