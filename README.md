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
