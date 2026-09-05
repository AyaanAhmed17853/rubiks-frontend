#include "pdb.h"
#include <fstream>
#include <iostream>

using namespace pieces;

namespace {

// Inverse of encodeCorners: reconstructs (cp, co) from a corner index.
void decodeIndex(int index, CornerArr& cp, CornerArr& co) {
    static const int fact[8] = {1, 1, 2, 6, 24, 120, 720, 5040};
    int permIndex = index / CORNER_ORI_STATES;
    int oriIndex = index % CORNER_ORI_STATES;

    int avail[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    int availLen = 8;
    for (int i = 0; i < NUM_CORNERS; ++i) {
        int f = fact[NUM_CORNERS - 1 - i];
        int d = permIndex / f;
        permIndex %= f;
        cp[i] = avail[d];
        for (int j = d; j < availLen - 1; ++j) avail[j] = avail[j + 1];
        --availLen;
    }

    int sum = 0;
    for (int i = 0; i < NUM_CORNERS - 1; ++i) {
        co[i] = oriIndex % 3;
        oriIndex /= 3;
        sum += co[i];
    }
    co[NUM_CORNERS - 1] = (3 - (sum % 3)) % 3;
}

} // namespace

void CornerPDB::loadOrBuild(const std::string& path) {
    if (load(path)) {
        std::cout << "Loaded corner pattern database from " << path << "\n";
        return;
    }
    std::cout << "Building corner pattern database (" << CORNER_STATES
              << " states)... this happens once and is cached to disk.\n";
    build();
    save(path);
    std::cout << "Saved corner pattern database to " << path << "\n";
}

bool CornerPDB::load(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    dist_.resize(CORNER_STATES);
    in.read(reinterpret_cast<char*>(dist_.data()), CORNER_STATES);
    if (!in || in.gcount() != static_cast<std::streamsize>(CORNER_STATES)) {
        dist_.clear();
        return false;
    }
    return true;
}

void CornerPDB::save(const std::string& path) const {
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(dist_.data()), CORNER_STATES);
}

void CornerPDB::build() {
    dist_.assign(CORNER_STATES, 255);
    dist_[0] = 0;

    std::vector<int> frontier = {0};
    int depth = 0;
    long long visited = 1;

    while (!frontier.empty()) {
        std::vector<int> next;
        next.reserve(frontier.size() * 4);
        for (int idx : frontier) {
            CornerArr cp, co;
            decodeIndex(idx, cp, co);
            for (int m = 0; m < 18; ++m) {
                CornerArr ncp = cp, nco = co;
                applyCornerMove(ncp, nco, m);
                int nidx = encodeCorners(ncp, nco);
                if (dist_[nidx] == 255) {
                    dist_[nidx] = static_cast<uint8_t>(depth + 1);
                    next.push_back(nidx);
                }
            }
        }
        visited += static_cast<long long>(next.size());
        std::cout << "  depth " << (depth + 1) << ": "
                  << next.size() << " new states (total " << visited << ")\n";
        frontier = std::move(next);
        ++depth;
    }
}
