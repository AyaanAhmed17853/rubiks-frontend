#include "pieces.h"

namespace pieces {

// Facelet positions for each corner slot, ordered so index 0 is the
// facelet that lies on the U/D axis in the solved cube.
// Slots: URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB
static const int cornerFacelet[NUM_CORNERS][3] = {
    {8, 9, 20},   // URF: U9 R1 F3
    {6, 18, 38},  // UFL: U7 F1 L3
    {0, 36, 47},  // ULB: U1 L1 B3
    {2, 45, 11},  // UBR: U3 B1 R3
    {29, 26, 15}, // DFR: D3 F9 R7
    {27, 44, 24}, // DLF: D1 L9 F7
    {33, 53, 42}, // DBL: D7 B9 L7
    {35, 17, 51}, // DRB: D9 R9 B7
};

static const char cornerColor[NUM_CORNERS][3] = {
    {'U', 'R', 'F'},
    {'U', 'F', 'L'},
    {'U', 'L', 'B'},
    {'U', 'B', 'R'},
    {'D', 'F', 'R'},
    {'D', 'L', 'F'},
    {'D', 'B', 'L'},
    {'D', 'R', 'B'},
};

// Facelet positions for each edge slot, primary (U/D, or F/B for the
// middle-layer edges) facelet first.
// Slots: UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
static const int edgeFacelet[NUM_EDGES][2] = {
    {5, 10},  // UR: U6 R2
    {7, 19},  // UF: U8 F2
    {3, 37},  // UL: U4 L2
    {1, 46},  // UB: U2 B2
    {32, 16}, // DR: D6 R8
    {28, 25}, // DF: D2 F8
    {30, 43}, // DL: D4 L8
    {34, 52}, // DB: D8 B8
    {23, 12}, // FR: F6 R4
    {21, 41}, // FL: F4 L6
    {50, 39}, // BL: B6 L4
    {48, 14}, // BR: B4 R6
};

static const char edgeColor[NUM_EDGES][2] = {
    {'U', 'R'}, {'U', 'F'}, {'U', 'L'}, {'U', 'B'},
    {'D', 'R'}, {'D', 'F'}, {'D', 'L'}, {'D', 'B'},
    {'F', 'R'}, {'F', 'L'}, {'B', 'L'}, {'B', 'R'},
};

static bool sameColorSet2(char a0, char a1, char b0, char b1) {
    return (a0 == b0 && a1 == b1) || (a0 == b1 && a1 == b0);
}

void decodeCorners(const Cube& c, CornerArr& cp, CornerArr& co) {
    for (int s = 0; s < NUM_CORNERS; ++s) {
        char a = c.f[cornerFacelet[s][0]];
        char b = c.f[cornerFacelet[s][1]];
        char d = c.f[cornerFacelet[s][2]];
        for (int k = 0; k < NUM_CORNERS; ++k) {
            const char* cc = cornerColor[k];
            // match {a,b,d} as a set against {cc[0],cc[1],cc[2]}
            bool m1 = (a == cc[0] && b == cc[1] && d == cc[2]) ||
                      (a == cc[0] && b == cc[2] && d == cc[1]) ||
                      (a == cc[1] && b == cc[0] && d == cc[2]) ||
                      (a == cc[1] && b == cc[2] && d == cc[0]) ||
                      (a == cc[2] && b == cc[0] && d == cc[1]) ||
                      (a == cc[2] && b == cc[1] && d == cc[0]);
            if (m1) {
                cp[s] = k;
                break;
            }
        }
        if (a == 'U' || a == 'D') co[s] = 0;
        else if (b == 'U' || b == 'D') co[s] = 1;
        else co[s] = 2;
    }
}

void decodeEdges(const Cube& c, EdgeArr& ep, EdgeArr& eo) {
    for (int s = 0; s < NUM_EDGES; ++s) {
        char a = c.f[edgeFacelet[s][0]];
        char b = c.f[edgeFacelet[s][1]];
        for (int k = 0; k < NUM_EDGES; ++k) {
            if (sameColorSet2(a, b, edgeColor[k][0], edgeColor[k][1])) {
                ep[s] = k;
                eo[s] = (a == edgeColor[k][0]) ? 0 : 1;
                break;
            }
        }
    }
}

int encodeCorners(const CornerArr& cp, const CornerArr& co) {
    static const int fact[8] = {1, 1, 2, 6, 24, 120, 720, 5040};
    int permIndex = 0;
    for (int i = 0; i < NUM_CORNERS; ++i) {
        int smaller = 0;
        for (int j = i + 1; j < NUM_CORNERS; ++j)
            if (cp[j] < cp[i]) ++smaller;
        permIndex += smaller * fact[NUM_CORNERS - 1 - i];
    }
    int oriIndex = 0;
    int pow3 = 1;
    for (int i = 0; i < NUM_CORNERS - 1; ++i) {
        oriIndex += co[i] * pow3;
        pow3 *= 3;
    }
    return permIndex * CORNER_ORI_STATES + oriIndex;
}

std::array<CornerArr, 18> cpMove;
std::array<CornerArr, 18> coMove;

void initMoveTables() {
    for (int m = 0; m < 18; ++m) {
        Cube c;
        c.applyMove(m);
        decodeCorners(c, cpMove[m], coMove[m]);
    }
}

void applyCornerMove(CornerArr& cp, CornerArr& co, int m) {
    CornerArr newcp, newco;
    for (int i = 0; i < NUM_CORNERS; ++i) {
        newcp[i] = cp[cpMove[m][i]];
        newco[i] = (co[cpMove[m][i]] + coMove[m][i]) % 3;
    }
    cp = newcp;
    co = newco;
}

int edgeLowerBound(const EdgeArr& ep, const EdgeArr& eo) {
    int misplaced = 0;
    for (int s = 0; s < NUM_EDGES; ++s)
        if (ep[s] != s || eo[s] != 0) ++misplaced;
    return (misplaced + 3) / 4;
}

} // namespace pieces
