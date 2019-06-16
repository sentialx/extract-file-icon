import { platform } from "os";

let getIcon: any;

if (platform() === "win32" || platform() === 'darwin') {
  getIcon = require("../build/Release/addon.node").getIcon;
}

const getFileIcon = (path: string, size: 16 | 32 | 64 | 256 = 64): Buffer => {
  if (!getIcon) return;
  return getIcon(path, size);
};

export = getFileIcon;
