This is a Tetris implementation in Unreal Engine 4.

## Design

### Actors

The `ABoard` renders the current `ATetromino`, the board's border, and the current cells, represented by `ACell`. The Board handles positioning the Tetromino, which involves determining its legal moves.

An `ATetromino` is represented by an `FShape` and an `FColor`. The Tetromino uses the Shape to construct itself from `ACell`s.

An `FShape` represents one of the the classical Tetris Tetrominoes. The particular shape determines the points that constitute it and each of their possible rotations.

An `ACell` contains a cube mesh, material, and `FColor`.