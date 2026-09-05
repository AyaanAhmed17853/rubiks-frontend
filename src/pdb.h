#pragma once
#include "pieces.h"
#include <cstdint>
#include <string>
#include <vector>

// Corner pattern database: for every reachable (corner permutation,
// corner orientation) pair, the minimum number of moves needed to solve
// the corners alone. Used as the IDA* heuristic.
class CornerPDB {
public:
    // Loads the PDB from `path` if it exists, otherwise builds it with a
    // breadth-first search from the solved state and writes it to `path`.
    void loadOrBuild(const std::string& path);

    inline int lookup(int cornerIndex) const { return dist_[cornerIndex]; }

private:
    std::vector<uint8_t> dist_;

    void build();
    bool load(const std::string& path);
    void save(const std::string& path) const;
};
