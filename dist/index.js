"use strict";
const os_1 = require("os");
let getIcon;
if (os_1.platform() === "win32") {
    getIcon = require("../build/Release/addon.node").getIcon;
}
const getFileIcon = (path, size = 64) => {
    if (!getIcon)
        return;
    return getIcon(path, size);
};
module.exports = getFileIcon;
//# sourceMappingURL=index.js.map