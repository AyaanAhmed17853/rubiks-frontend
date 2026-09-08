#include "third_party/httplib.h"

#include "cube.h"
#include "pdb.h"
#include "pieces.h"
#include "solver.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    // Required by the existing solver/PDB implementation.
    pieces::initMoveTables();

    // Load the same PDB used by the console solver.
    CornerPDB pdb;
    pdb.loadOrBuild("corner_pdb.dat");

    httplib::Server svr;

    // Simple health check.
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
    res.set_content("OK", "text/plain");
    });
    // The request body is the scramble itself.
    // Example body:
    // R U2 F' L
    svr.Post("/solve", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:5173");
        const std::string& scramble = req.body;

        std::cout << "\nReceived scramble: " << scramble << "\n";

        Cube cube;

        try {
            cube.applyScramble(scramble);
        } catch (const std::exception& e) {
            std::ostringstream json;
            json << R"({"success":false,"error":")"
                 << e.what()
                 << R"("})";

            res.status = 400;
            res.set_content(json.str(), "application/json");
            return;
        }

        const std::string facelets = cube.toString();

        // Same behavior as the existing console program.
        if (cube.isSolved()) {
            res.set_content(
                R"({"success":true,"facelets":")" +
                    facelets +
                    R"(","solution":"","moveCount":0,"timeSeconds":0,"verified":true})",
                "application/json"
            );
            return;
        }

        Solver solver(pdb);

        auto t0 = std::chrono::steady_clock::now();

        bool found = false;
        const int maxDepth = 20;

        std::vector<int> solution =
            solver.solve(cube, maxDepth, found);

        auto t1 = std::chrono::steady_clock::now();

        double seconds =
            std::chrono::duration<double>(t1 - t0).count();

        if (!found) {
            std::ostringstream json;

            json << R"({"success":false,"error":"No solution found within )"
                 << maxDepth
                 << R"( moves"})";

            res.status = 422;
            res.set_content(json.str(), "application/json");
            return;
        }

        // Convert the solution move indices back into standard notation.
        std::ostringstream solutionStream;

        for (size_t i = 0; i < solution.size(); ++i) {
            if (i > 0) {
                solutionStream << ' ';
            }

            solutionStream << Cube::moveStrFromIndex(solution[i]);
        }

        const std::string solutionString =
            solutionStream.str();

        // Verify exactly as the existing console application does.
        Cube check = cube;

        for (int move : solution) {
            check.applyMove(move);
        }

        const bool verified = check.isSolved();

        std::ostringstream json;

        json << R"({"success":true)"
             << R"(,"facelets":")" << facelets << R"(")"
             << R"(,"solution":")" << solutionString << R"(")"
             << R"(,"moveCount":)" << solution.size()
             << R"(,"timeSeconds":)" << seconds
             << R"(,"verified":)" << (verified ? "true" : "false")
             << '}';

        res.set_content(json.str(), "application/json");
    });

    std::cout << "Server starting on http://localhost:8080\n";

    if (!svr.listen("localhost", 8080)) {
        std::cerr << "Failed to start server\n";
        return 1;
    }

    return 0;
}