#pragma once
#include "cube.h"
#include <array>

// Cubie-level representation of the 8 corners and 12 edges, derived purely
// from the facelet move tables in cube.cpp. Used to build the corner
// pattern database and the cheap edge-placement heuristic for IDA*.
namespace pieces {

// Corner slot order: URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB
constexpr int NUM_CORNERS = 8;
// Edge slot order: UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
constexpr int NUM_EDGES = 12;

constexpr int CORNER_PERM_STATES = 40320;       // 8!
constexpr int CORNER_ORI_STATES = 2187;         // 3^7
constexpr int CORNER_STATES = CORNER_PERM_STATES * CORNER_ORI_STATES; // 88,179,840

using CornerArr = std::array<int, NUM_CORNERS>;
using EdgeArr = std::array<int, NUM_EDGES>;

// Reads the current corner permutation/orientation from a cube's facelets.
void decodeCorners(const Cube& c, CornerArr& cp, CornerArr& co);

// Reads the current edge permutation/orientation from a cube's facelets.
void decodeEdges(const Cube& c, EdgeArr& ep, EdgeArr& eo);

// Encodes (cp, co) into a single index in [0, CORNER_STATES).
int encodeCorners(const CornerArr& cp, const CornerArr& co);

// Per-move effect tables (index 0..17), computed once from the solved cube.
extern std::array<CornerArr, 18> cpMove;
extern std::array<CornerArr, 18> coMove;

void initMoveTables();

// Applies move m to a corner state (cp, co) in place.
void applyCornerMove(CornerArr& cp, CornerArr& co, int m);

// Lower bound on remaining moves based on how many edges are out of place
// (each move can fix at most 4 edges).
int edgeLowerBound(const EdgeArr& ep, const EdgeArr& eo);

} // namespace pieces
