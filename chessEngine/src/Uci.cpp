//
// Created by guykn on 1/6/2021.
//

#include <Uci.h>
#include "Bitboards.h"
#include <fstream>
#include <string>
#include <sstream>
#include <Search.h>
#include <Benchmarks.h>
#include "TransPositionTable.h"
#include "init.h"

using std::cout;
using std::cin;
using std::string;
using std::ofstream;
using std::ios;
using std::ios_base;
using std::getline;
using std::stringstream;

using namespace Chess;
namespace Uci {
    Search search{};

    const string ERROR_TAG = "info string ERROR: ";

    static string LOG_TAG = "info string ";

    void error(const string &message) {
        cout << ERROR_TAG << message << "\n";
    }

    void log(const string &message) {
        cout << LOG_TAG << message << "\n";
    }


    void setPosition(stringstream &uciCommand) {
        string command;
        if (uciCommand >> command) {
            if (command == "startpos") {
                search.chessBoard.resetPosition();
                if (uciCommand >> command && command == "moves") {
                    if (search.chessBoard.doMoves(uciCommand)) {
                        //sucsess
                    } else {
                        error("error with doMoves");
                        exit(77);
                    }
                }
            } else if (command == "fen") {
                stringstream fenStream;
                string str;
                bool parseMoves = false;
                while (uciCommand >> str) {
                    if (str == "moves") {
                        parseMoves = true;
                        break;
                    }
                    fenStream << str << " ";
                }
                string fen = fenStream.str();
                if (search.chessBoard.parseFen(fen)) {
                    //sucsess
                    if (parseMoves) {
                        if (search.chessBoard.doMoves(uciCommand)) {
                        } else {
                            error("error going to pos");
                            exit(0);
                        }
                    }
                } else {
                    error("can't parse fen: " + fen);
                }
            } else {
                error("setposition command: '" + command + "' . not found");
            }
        } else {
            error("setposition command was given empty line");
        }
    }

    static const int UCI_DEPTH = 7;

    void uciMain() {
        for (string line; getline(cin, line);) {
            stringstream lineStream{line};
            string firstWord;
            if (lineStream >> firstWord) {
                if (firstWord == "uci") {
                    if constexpr (FOR_RELEASE) {
                        cout << "id name UCI release" << "\n"
                             << "id author Guy Knaan" << "\n"
                             << "option name OwnBook type check default true" << "\n"
                             << "uciok\n";
                    } else {
                        cout << "id name UCI debug" << "\n"
                             << "id author Guy Knaan" << "\n"
                             << "option name OwnBook type check default true" << "\n"
                             << "uciok\n";
                    }
                } else if (firstWord == "quit") {
                    return;
                } else if (firstWord == "isready") {
                    initAll();
                    cout << "readyok\n";
                } else if (firstWord == "debug") {
                    //todo
                } else if (firstWord == "setoption") {
                    //todo
                } else if (firstWord == "ucinewgame") {
                    transPositionTable.clear();
                } else if (firstWord == "position") {
                    setPosition(lineStream);
                } else if (firstWord == "go") {
                    search.chessBoard.printMoves<CAPTURES>();
                    Stopwatch stopwatch{};
                    stopwatch.start();
                    Move bestMove = search.bestMove([&stopwatch](auto) {
                        return stopwatch.getSecondsElapsed() > 2;
                    });
                    double timeElapsed = stopwatch.getSecondsElapsed();
                    int numNodes = search.getNumNodes();
                    int nodesPerSecond = numNodes / timeElapsed;
                    Score score = search.getScore();

                    if (score > SCORE_KNOWN_WIN) {
                        cout << "info score mate " << (SCORE_MATE - score + 1) / 2;
                    } else if (score < SCORE_KNOWN_LOSS) {
                        cout << "info score mate " << -(score - SCORE_MATED + 1) / 2;
                    } else {
                        cout << "info score cp " << score;
                    }
                    cout << "\n";
                    cout << "bestmove " << bestMove << "\n";
                    cout << "info depth " << search.getDepth();
                    cout << " hashfull " << transPositionTable.perMillEntriesUsed();
                    cout << " time " << static_cast<int>(timeElapsed * 1000);
                    cout << " nodes " << numNodes;
                    cout << " nps " << nodesPerSecond << "\n";
                    cout << LOG_TAG << "Hash Table Entries deleted:" << transPositionTable.entriesDeleted() << "\n";
                } else if (firstWord == "stop") {
                    error("not supported");//todo
                } else if (firstWord == "ponderhit") {
                    //todo
                } else {
                    error("command not found.");
                }
            }
        }
        log("exiting!");
    }
}
