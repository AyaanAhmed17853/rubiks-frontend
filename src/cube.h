#pragma once
#include <array>
#include <string>

// Facelet layout (54 stickers, 9 per face, row-major within each face):
//   U = 0..8, R = 9..17, F = 18..26, D = 27..35, L = 36..44, B = 45..53
// Each face's 9 stickers are numbered:
//   0 1 2
//   3 4 5
//   6 7 8
class Cube {
public:
    std::array<char, 54> f;

    Cube();

    void reset();
    bool isSolved() const;
    std::string toString() const;

    // moveIndex: 0..17 -> U,U2,U',R,R2,R',F,F2,F',D,D2,D',L,L2,L',B,B2,B'
    void applyMove(int moveIndex);
    void applyMoveStr(const std::string& mv);

    // Parses a whitespace-separated scramble (e.g. "R U2 F' L") and applies it.
    void applyScramble(const std::string& scramble);

    static int moveIndexFromStr(const std::string& mv);
    static std::string moveStrFromIndex(int idx);
    static int inverseMove(int idx);

    // Returns true if moves a and b act on the same face (e.g. R and R2).
    static bool sameFace(int a, int b);

private:
    void cycle4(int a, int b, int c, int d);
    void turnFace(int face); // face: 0=U,1=R,2=F,3=D,4=L,5=B (clockwise quarter turn)
};
