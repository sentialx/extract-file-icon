import { platform } from "os";

let getIcon: any;

if (platform() === "win32") {
  getIcon = require("../build/Release/addon.node").getIcon;
}

const getFileIcon = (path: string): Buffer => {
  if (!getIcon) return;
  return getIcon(path);
};

export default getFileIcon;
