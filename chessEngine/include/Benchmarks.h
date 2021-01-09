//
// Created by guykn on 1/4/2021.
//

#ifndef CHESS_BENCHMARKS_H
#define CHESS_BENCHMARKS_H

#include <chrono>
#include <thread>

namespace Chess {
    using namespace std::chrono;
    class Stopwatch {
        using time_point = high_resolution_clock::time_point;

        time_point startTime;

        static inline time_point getTime() {
            return high_resolution_clock::now();
        }
    public:
        inline void start() {
            startTime = getTime();
        }

        inline double getSecondsElapsed(){
            time_point now = getTime();
            return duration_cast<duration<double>>(now-startTime).count();
        }

        inline static void test(){
            Stopwatch stopwatch{};
            stopwatch.start();
            std::this_thread::sleep_for(1000ms);
            cout << "slept for 1 seconds. Seconds: "<< stopwatch.getSecondsElapsed() << "\n";
        }
    };

    void doBenchmarks();
}
#endif //CHESS_BENCHMARKS_H
