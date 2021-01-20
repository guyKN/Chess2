//
// Created by guykn on 1/19/2021.
//

#include <Bitboards.h>
#include <TransPositionTable.h>
#include <ExchangeEvaluation.h>
#include "init.h"
namespace Chess{
    void initAll() {
        initPieceMoveLookup();
        if (!transPositionTable.isInitialized()){
            transPositionTable = TransPositionTable::fromSize(TransPositionTable::TRANSPOSITION_TABLE_SIZE);
            cout << "num TT entries: " << transPositionTable.getNumEntries() << "\n";
        }
        zobristData.init();
        initExchangeLookup();
    }
}
