"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const os_1 = require("os");
let getIcon;
if (os_1.platform() === 'win32') {
    getIcon = require("bindings")("addon");
}
const getFileIcon = (path) => {
    if (!getIcon)
        return;
    return getIcon(path);
};
exports.getFileIcon = getFileIcon;
//# sourceMappingURL=index.js.map