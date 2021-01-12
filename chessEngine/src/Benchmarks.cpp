//
// Created by guykn on 1/4/2021.
//



#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"
#include "Search.h"

namespace Chess {
    using namespace std::chrono;
    using std::cout;

    class Stopwatch {
        using time_point = high_resolution_clock::time_point;

        time_point startTime;

        static time_point getTime() {
            return high_resolution_clock::now();
        }
    public:
        void start() {
            startTime = getTime();
        }

        double getSecondsElapsed(){
            time_point now = getTime();
            return duration_cast<duration<double>>(now-startTime).count();
        }

        static void test(){
            Stopwatch stopwatch{};
            stopwatch.start();
            std::this_thread::sleep_for(1000ms);
            cout << "slept for 1 seconds. Seconds: "<< stopwatch.getSecondsElapsed() << "\n";
        }
    };

    void alphaBetaSearchBenchmark(int depth){
        cout << "Preforming benchmark for AlphaBeta Search ..\n";
        ChessBoard chessBoard{};
        Search search{chessBoard};
        Stopwatch stopwatch{};
        stopwatch.start();
        Move move = search.bestMove(depth);
        double timeElapsed = stopwatch.getSecondsElapsed();
        int numNodes = search.getNumNodes();
        int numLeaves = search.getNumLeaves();
        double nodesPerSecond = numNodes/timeElapsed;
        cout << std::dec << "Done with benchmark for AlphaBeta Search\n" <<
        "best bestMove_: " << move <<  "\n" <<
        "Depth: " << depth << "\n" <<
        "Time elapsed: " << timeElapsed << "\n" <<
        "num nodes searched: " << std::scientific << numNodes << "\n" <<
        "num leaves searched: " << numLeaves << "\n" <<
        "nodes per second: " << nodesPerSecond << "\n"
        << std::dec;
    }

    void doBenchmarks() {
        if(!FOR_RELEASE){
            cout << "WARNING: doing benchmarks in debug code. Probably not recomended\n";
        }
        Stopwatch stopwatch;
        stopwatch.start();
        initAll();
        double timeElapsed = stopwatch.getSecondsElapsed();
        cout << "time for initLookUpTables: " << timeElapsed << "sec" << "\n";
        alphaBetaSearchBenchmark(4);
    }
}
