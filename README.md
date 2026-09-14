# Rubik's Cube Solver (IDA* + Corner Pattern Database)

## Layout

- `src/cube.h` / `src/cube.cpp` — 54-char facelet representation (`U`=0-8,
  `R`=9-17, `F`=18-26, `D`=27-35, `L`=36-44, `B`=45-53) and the 18 face
  turns (`U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'`).
- `src/pieces.h` / `src/pieces.cpp` — derives the 8-corner permutation +
  orientation from the facelet array, encodes it as an index in
  `[0, 88179840)`, and computes a cheap admissible edge-placement bound.
- `src/pdb.h` / `src/pdb.cpp` — corner pattern database. On first run it
  runs a BFS from the solved state over the corner state space (88,179,840
  states) and caches the result to `corner_pdb.dat` (~84 MB) so later runs
  load instantly.
- `src/solver.h` / `src/solver.cpp` — IDA* using `max(cornerPDB, edgeBound)`
  as the heuristic.
- `src/main.cpp` — prompts for a scramble, applies it, **discards the
  scramble string** (the "amnesia" requirement — the program afterwards
  only has the 54-char facelet array), then solves and verifies.

## Building

This environment has no C++ toolchain installed, so the code has not been
compiled here. With CMake + a C++17 compiler:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

Or directly with g++:

```sh
g++ -std=c++17 src/main.cpp src/cube.cpp src/pieces.cpp src/pdb.cpp src/solver.cpp -o rubiks_solver.exe
```

## Running

```sh
./rubiks_solver
Enter scramble sequence (e.g. "R U2 F' L"): R U2 F' L D B2
```

The first run builds and caches the corner PDB (a couple of minutes,
~84 MB on disk); subsequent runs load it instantly.

## Validating the move/encoding logic

`tests/validate.py` mirrors the cube/move/corner-encoding logic in pure
Python and checks (via 200 random scrambles):

- every move has order dividing 4,
- a scramble followed by the reversed inverse moves restores the solved
  state,
- corner/edge decode invariants always hold,
- the cubie-level composition formula used to build `cpMove`/`coMove`
  (and hence the PDB) matches direct decoding of the scrambled cube.


## Scope / limitations

- The heuristic only includes a full pattern database for the **corners**
  (Korf's algorithm also uses two 6-edge PDBs for much stronger pruning).
  This solver will find optimal-ish solutions quickly for scrambles up to
  roughly 12-14 moves; longer/20-move scrambles may take noticeably longer
  or hit the 20-move search cap in `main.cpp`.
- Adding edge pattern databases (`UR UF UL UB DR DF` and `DL DB FR FL BL
  BR`, 6 edges each, ~42M states per table) would be the natural next step
  to speed up harder scrambles.


g++ *.cpp -O2 -std=c++17 -o rubiks_solver
rubiks_solver.exe --scramble "F R U R' U' F'"