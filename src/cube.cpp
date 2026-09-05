#include "cube.h"
#include <sstream>
#include <stdexcept>

Cube::Cube() { reset(); }

void Cube::reset() {
    static const char solved[55] =
        "UUUUUUUUU"  // U
        "RRRRRRRRR"  // R
        "FFFFFFFFF"  // F
        "DDDDDDDDD"  // D
        "LLLLLLLLL"  // L
        "BBBBBBBBB"; // B
    for (int i = 0; i < 54; ++i) f[i] = solved[i];
}

bool Cube::isSolved() const {
    for (int face = 0; face < 6; ++face) {
        char c = f[face * 9];
        for (int i = 1; i < 9; ++i)
            if (f[face * 9 + i] != c) return false;
    }
    return true;
}

std::string Cube::toString() const {
    return std::string(f.begin(), f.end());
}

// Content moves a -> b -> c -> d -> a
void Cube::cycle4(int a, int b, int c, int d) {
    char t = f[a];
    f[a] = f[d];
    f[d] = f[c];
    f[c] = f[b];
    f[b] = t;
}

void Cube::turnFace(int face) {
    switch (face) {
        case 0: // U
            cycle4(0, 2, 8, 6);
            cycle4(1, 5, 7, 3);
            cycle4(9, 18, 36, 45);
            cycle4(10, 19, 37, 46);
            cycle4(11, 20, 38, 47);
            break;
        case 1: // R
            cycle4(9, 11, 17, 15);
            cycle4(10, 14, 16, 12);
            cycle4(2, 51, 29, 20);
            cycle4(5, 48, 32, 23);
            cycle4(8, 45, 35, 26);
            break;
        case 2: // F
            cycle4(18, 20, 26, 24);
            cycle4(19, 23, 25, 21);
            cycle4(6, 9, 29, 44);
            cycle4(7, 12, 28, 41);
            cycle4(8, 15, 27, 38);
            break;
        case 3: // D
            cycle4(27, 29, 35, 33);
            cycle4(28, 32, 34, 30);
            cycle4(24, 15, 51, 42);
            cycle4(25, 16, 52, 43);
            cycle4(26, 17, 53, 44);
            break;
        case 4: // L
            cycle4(36, 38, 44, 42);
            cycle4(37, 41, 43, 39);
            cycle4(0, 18, 27, 53);
            cycle4(3, 21, 30, 50);
            cycle4(6, 24, 33, 47);
            break;
        case 5: // B
            cycle4(45, 47, 53, 51);
            cycle4(46, 50, 52, 48);
            cycle4(0, 42, 35, 11);
            cycle4(1, 39, 34, 14);
            cycle4(2, 36, 33, 17);
            break;
    }
}

void Cube::applyMove(int moveIndex) {
    int face = moveIndex / 3;
    int var = moveIndex % 3; // 0 = quarter, 1 = half, 2 = inverse quarter
    int times = (var == 0) ? 1 : (var == 1) ? 2 : 3;
    for (int i = 0; i < times; ++i) turnFace(face);
}

int Cube::moveIndexFromStr(const std::string& mv) {
    if (mv.empty()) throw std::invalid_argument("empty move");
    static const std::string faces = "URFDLB";
    size_t pos = faces.find(mv[0]);
    if (pos == std::string::npos) throw std::invalid_argument("bad move face: " + mv);
    int var = 0; // quarter turn
    if (mv.size() > 1) {
        if (mv[1] == '2') var = 1;
        else if (mv[1] == '\'') var = 2;
        else throw std::invalid_argument("bad move suffix: " + mv);
    }
    return static_cast<int>(pos) * 3 + var;
}

std::string Cube::moveStrFromIndex(int idx) {
    static const std::string faces = "URFDLB";
    int face = idx / 3;
    int var = idx % 3;
    std::string s(1, faces[face]);
    if (var == 1) s += '2';
    else if (var == 2) s += '\'';
    return s;
}

int Cube::inverseMove(int idx) {
    int face = idx / 3;
    int var = idx % 3;
    int invVar = (var == 1) ? 1 : (2 - var); // 0<->2, 1->1
    return face * 3 + invVar;
}

bool Cube::sameFace(int a, int b) {
    return a / 3 == b / 3;
}

void Cube::applyMoveStr(const std::string& mv) {
    applyMove(moveIndexFromStr(mv));
}

void Cube::applyScramble(const std::string& scramble) {
    std::istringstream iss(scramble);
    std::string tok;
    while (iss >> tok) applyMoveStr(tok);
}
