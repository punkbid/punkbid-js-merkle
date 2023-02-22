export declare class MerkleTree {
  constructor(ids: number[]);
  root: string;
  proof(id: number): string[];
}

export default MerkleTree;
