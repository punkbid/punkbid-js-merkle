import test, { describe } from "node:test";
import assert from "node:assert";
import MerkleTree from "../src/index.js";

describe("MerkleTree", () => {
  test("even number of ids", () => {
    const tree = new MerkleTree([1, 2]);

    assert.strictEqual(
      "0x2a171b5bcd1449348c3e09a5424946b5e6d6f5471221941d585131d673952ee4",
      tree.root
    );
    assert.deepStrictEqual(
      ["0x405787fa12a823e0f2b7631cc41b3ba8828b3321ca811111fa75cd3aa3bb5ace"],
      tree.proof(1)
    );
    assert.deepStrictEqual(
      ["0xb10e2d527612073b26eecdfd717e6a320cf44b4afac2b0732d9fcbe2b7fa0cf6"],
      tree.proof(2)
    );
  });

  test("odd number of ids", () => {
    const tree = new MerkleTree([1, 2, 3]);

    assert.strictEqual(
      "0x4cdbcd942bd29b80bbd5eb9929ec8d0ea9c97d2690f9d2f8318390505ec1a769",
      tree.root
    );
    assert.deepStrictEqual(
      [
        "0x405787fa12a823e0f2b7631cc41b3ba8828b3321ca811111fa75cd3aa3bb5ace",
        "0xc2575a0e9e593c00f959f8c92f12db2869c3395a3b0502d05e2516446f71f85b",
      ],
      tree.proof(1)
    );
    assert.deepStrictEqual(
      [
        "0xb10e2d527612073b26eecdfd717e6a320cf44b4afac2b0732d9fcbe2b7fa0cf6",
        "0xc2575a0e9e593c00f959f8c92f12db2869c3395a3b0502d05e2516446f71f85b",
      ],
      tree.proof(2)
    );
    assert.deepStrictEqual(
      ["0x2a171b5bcd1449348c3e09a5424946b5e6d6f5471221941d585131d673952ee4"],
      tree.proof(3)
    );
  });

  test("undefined ids", () => {
    assert.throws(() => new MerkleTree(), {
      name: "TypeError",
      message: "Array expected",
    });
  });

  test("empty ids array", () => {
    assert.throws(() => new MerkleTree([]), {
      name: "TypeError",
      message: "Invalid array length",
    });
  });

  test("invalid id", () => {
    assert.throws(() => new MerkleTree([10000]), {
      name: "TypeError",
      message: "Invalid id",
    });
    assert.throws(() => new MerkleTree([-1]), {
      name: "TypeError",
      message: "Invalid id",
    });
    assert.throws(() => new MerkleTree(["test"]), {
      name: "Error",
      message: "A number was expected",
    });
  });

  test("proof of undefined id", () => {
    const tree = new MerkleTree([1, 2]);
    assert.throws(() => tree.proof(), {
      name: "TypeError",
      message: "Number expected",
    });
  });
});
