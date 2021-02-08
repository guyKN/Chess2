//
// Created by guykn on 1/4/2021.
//



#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>
#include <init.h>
#include "Bitboards.h"
#include "ChessBoard.h"
#include "Search.h"
#include "TransPositionTable.h"

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

    void alphaBetaSearchBenchmark(int depth, bool iterativeDeepening){
        cout << "Preforming benchmark for AlphaBeta Search ..\n";
        ChessBoard chessBoard{};
        chessBoard.parseFen("r2q1rk1/3nbppp/p2pbn2/1p2p3/4P1P1/1NN1BP2/PPPQ3P/2KR1B1R w - - 0 12");
        Search search{chessBoard};
        Stopwatch stopwatch{};
        stopwatch.start();
        Move move;

        if(iterativeDeepening){
            move = search.bestMove([depth](int currentDepth){
                return currentDepth > depth;
            });
        } else{
            move = search.bestMoveAtDepth(depth);
        }
        double timeElapsed = stopwatch.getSecondsElapsed();
        int numNodes = search.getNumNodes();
        int numLeaves = search.getNumLeaves();
        double nodesPerSecond = numNodes/timeElapsed;

        cout << std::dec << "Done with benchmark for AlphaBeta Search\n" <<
        "best move: " << move <<  "\n" <<
        "eval: " << search.getScore() << "\n"
        "Depth: " << depth << "\n" <<
        "Time elapsed: " << timeElapsed << "\n" <<
        "num nodes searched: " << std::scientific << numNodes << "\n" <<
        "num leaves searched: " << numLeaves << "\n" <<
        "nodes per second: " << nodesPerSecond << "\n" <<
        "transposition table entries used per mill: " << transPositionTable.perMillEntriesUsed()<<"\n" <<
        "Transposition table entries deleted: " << transPositionTable.entriesDeleted() << "\n"
        << std::dec;
    }

    void perftBenchmarks(int depth){
        Search search{ChessBoard()};
        Stopwatch stopwatch{};
        stopwatch.start();
        uint64_t numPositions = search.perft(depth);
        double timeElapsed = stopwatch.getSecondsElapsed();
        double nps = numPositions/timeElapsed;
        cout << "doing perft" << "\n" <<
         "time: " << timeElapsed << "\n" <<
        "positions: " << numPositions << "\n" <<
        "nodes per second: " << nps << "\n";
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

        alphaBetaSearchBenchmark(9, true);

        //perftBenchmarks(7);

    }
}