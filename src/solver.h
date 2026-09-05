#pragma once
#include "cube.h"
#include "pdb.h"
#include <vector>

// IDA* search guided by a corner pattern database (optimal for the corners
// alone) plus a simple admissible edge-placement bound.
class Solver {
public:
    explicit Solver(const CornerPDB& pdb) : pdb_(pdb) {}

    // Returns a sequence of move indices that solves `cube`, or an empty
    // vector if `cube` is already solved, or std::nullopt-like (returns
    // {-1} sentinel via `found` flag) if no solution was found within
    // maxDepth.
    std::vector<int> solve(Cube cube, int maxDepth, bool& found);

private:
    const CornerPDB& pdb_;
    Cube cube_;
    std::vector<int> path_;

    int heuristic() const;
    bool dfs(int g, int threshold, int lastMove, int& nextThreshold);
};
