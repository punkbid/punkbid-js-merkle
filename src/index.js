import { createRequire } from "module";
const require = createRequire(import.meta.url);
const MerkleTree = require("../build/Release/merkle.node");
export default MerkleTree;
