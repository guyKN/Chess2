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
                    }
                }
            } else if (command == "fen") {
                stringstream fenStream;
                string str;
                bool parseMoves = false;
                while (uciCommand >> str) {
                    if (str == "moves") {
                        parseMoves = true;
                    }
                    fenStream << str << " ";
                }
                string fen = fenStream.str();
                if (search.chessBoard.parseFen(fen)) {
                    //sucsess
                    if (parseMoves) {
                        if (search.chessBoard.doMoves(uciCommand)) {
                            log("sucsess going to pos");
                        } else {
                            error("error going to pos");
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

    void uciMain() {
        for (string line; getline(cin, line);) {
            stringstream lineStream{line};
            string firstWord;
            if (lineStream >> firstWord) {
                if (firstWord == "uci") {
                    cout << "id name EngineTest" << "\n"
                         << "id author Guy Knaan" << "\n"
                         << "option name OwnBook type check default true" << "\n"
                         << "uciok\n";
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
                    log("trying to go to position");
                    setPosition(lineStream);
                    log("success in going to position");
                } else if (firstWord == "go") {
                    log("searching for best bestMove");
                    Stopwatch stopwatch{};
                    stopwatch.start();
                    Move bestMove = search.bestMove(7);
                    double timeElapsed = stopwatch.getSecondsElapsed();
                    int numNodes = search.getNumNodes();
                    int nodesPerSecond = numNodes / timeElapsed;
                    Score score = search.getScore();
                    cout << "info score cp " << score << "\n";
                    cout << "info time " << static_cast<int>(timeElapsed * 1000) << "\n";
                    cout << "info nodes " << numNodes << "\n";
                    cout << "info nps " << nodesPerSecond << "\n";
                    cout << "bestmove " << bestMove << "\n";
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
