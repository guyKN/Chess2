//
// Created by guykn on 1/6/2021.
//

#include <Uci.h>

using std::cout;
using std::cin;
using std::string;

void uciMain() {
    string str;
    while (cin >> str){
        if (str == "uci"){
            cout << "id name BlockFish" << "\n"
            << "id author Guy Knaan" << "\n";
        } else{
            cout << str;
        }
    }
}
