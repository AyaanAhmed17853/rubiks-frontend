#include "solver.h"
#include "pieces.h"
#include <limits>

using namespace pieces;

int Solver::heuristic() const {
    CornerArr cp, co;
    decodeCorners(cube_, cp, co);
    int hCorners = pdb_.lookup(encodeCorners(cp, co));

    EdgeArr ep, eo;
    decodeEdges(cube_, ep, eo);
    int hEdges = edgeLowerBound(ep, eo);

    return hCorners > hEdges ? hCorners : hEdges;
}

bool Solver::dfs(int g, int threshold, int lastMove, int& nextThreshold) {
    int h = heuristic();
    int fcost = g + h;
    if (fcost > threshold) {
        if (fcost < nextThreshold) nextThreshold = fcost;
        return false;
    }
    if (h == 0) return true; // both corners and edges solved => cube solved

    for (int m = 0; m < 18; ++m) {
        if (lastMove != -1 && Cube::sameFace(m, lastMove)) continue;

        cube_.applyMove(m);
        path_.push_back(m);
        if (dfs(g + 1, threshold, m, nextThreshold)) return true;
        path_.pop_back();
        cube_.applyMove(Cube::inverseMove(m));
    }
    return false;
}

std::vector<int> Solver::solve(Cube cube, int maxDepth, bool& found) {
    cube_ = cube;
    path_.clear();
    found = false;

    int threshold = heuristic();
    if (threshold == 0) {
        found = true;
        return path_;
    }

    while (threshold <= maxDepth) {
        int nextThreshold = std::numeric_limits<int>::max();
        if (dfs(0, threshold, -1, nextThreshold)) {
            found = true;
            return path_;
        }
        if (nextThreshold == std::numeric_limits<int>::max()) break;
        threshold = nextThreshold;
    }
    return {};
}
