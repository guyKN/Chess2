//
// Created by guykn on 1/6/2021.
//

#ifndef CHESS_UCI_H
#define CHESS_UCI_H

#include <iostream>

using std::cout;
using std::cin;
using std::string;
namespace Uci {
    void uciMain();
    void log(const string&);
    void error(const string&);
}
#endif //CHESS_UCI_H
