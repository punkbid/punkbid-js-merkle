This package provides a minimal and performance oriented implementation of a merkle tree for the punk.bid project.

## Installation

```
npm install @punk.bid/merkle
```

## Usage

```javascript
import MerkleTree from "@punk.bid/merkle";

// MerkleTree takes an array of integers
// between 0 and 9999
const tree = new MerkleTree([1, 7, 9]);
const root = tree.root;
const proof = tree.proof(7);
```
