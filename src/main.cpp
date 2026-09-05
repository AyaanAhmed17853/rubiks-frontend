#include "cube.h"
#include "pdb.h"
#include "pieces.h"
#include "solver.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

static void printCube(const Cube& c) {
    auto& f = c.f;
    auto row = [&](int face, int start) {
        std::cout << f[face * 9 + start] << ' ' << f[face * 9 + start + 1]
                  << ' ' << f[face * 9 + start + 2];
    };
    for (int r = 0; r < 3; ++r) {
        std::cout << "      ";
        row(0, r * 3);
        std::cout << "\n";
    }
    for (int r = 0; r < 3; ++r) {
        row(4, r * 3); std::cout << "  ";
        row(2, r * 3); std::cout << "  ";
        row(1, r * 3); std::cout << "  ";
        row(5, r * 3);
        std::cout << "\n";
    }
    for (int r = 0; r < 3; ++r) {
        std::cout << "      ";
        row(3, r * 3);
        std::cout << "\n";
    }
}

int main() {
    pieces::initMoveTables();

    CornerPDB pdb;
    pdb.loadOrBuild("corner_pdb.dat");

    std::cout << "\nEnter scramble sequence (e.g. \"R U2 F' L\"): ";

    Cube cube;
    {
        // The scramble string lives only in this block. Once it goes out
        // of scope the program retains nothing but the resulting cube
        // state -- it has no memory of which moves produced it.
        std::string scramble;
        std::getline(std::cin, scramble);
        try {
            cube.applyScramble(scramble);
        } catch (const std::exception& e) {
            std::cerr << "Invalid scramble: " << e.what() << "\n";
            return 1;
        }
        scramble.clear();
    }

    std::cout << "\nScrambled cube:\n";
    printCube(cube);
    std::cout << "\nFacelets: " << cube.toString() << "\n";

    if (cube.isSolved()) {
        std::cout << "\nThe cube is already solved.\n";
        return 0;
    }

    std::cout << "\nSolving (IDA* with corner pattern database)...\n";

    Solver solver(pdb);
    auto t0 = std::chrono::steady_clock::now();
    bool found = false;
    const int maxDepth = 20;
    std::vector<int> solution = solver.solve(cube, maxDepth, found);
    auto t1 = std::chrono::steady_clock::now();
    double seconds = std::chrono::duration<double>(t1 - t0).count();

    if (!found) {
        std::cout << "No solution found within " << maxDepth
                  << " moves (try a shorter scramble).\n";
        return 1;
    }

    std::cout << "Solution (" << solution.size() << " moves, " << seconds
              << "s): ";
    for (int m : solution) std::cout << Cube::moveStrFromIndex(m) << ' ';
    std::cout << "\n";

    // Verify.
    Cube check = cube;
    for (int m : solution) check.applyMove(m);
    std::cout << (check.isSolved() ? "Verified: cube is solved.\n"
                                    : "ERROR: solution does not solve the cube!\n");

    return 0;
}
