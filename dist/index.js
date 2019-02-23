"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const { getIcon } = require("bindings")("addon");
const getFileIcon = (path) => {
    return getIcon(path);
};
exports.getFileIcon = getFileIcon;
//# sourceMappingURL=index.js.map