const { getIcon } = require("bindings")("addon");

const getFileIcon = (path: string): Buffer => {
  return getIcon(path);
};

export { getFileIcon };
