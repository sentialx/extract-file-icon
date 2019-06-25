# extract-file-icon

Get associated file icon for Node.js.

# Installation

To install this package, just run

```bash
$ npm install extract-file-icon
```

# Quick start

The following example shows how to get the currently focused window's title and hide it.

```javascript
const fileIcon = require("extract-file-icon");

const icon = fileIcon('path', 32); // Returns PNG buffer of 32x32 file icon at given path. 
```

# Documentation

## Default exported method `getFileIcon(path: string, size: 16 | 32 | 64 | 256 = 64)`
- `path` string
- `size` number - by default its value is 64. On Windows it can be only 16, 32, 64 or 256.
