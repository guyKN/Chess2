//
// Created by guykn on 1/11/2021.
//

#include "TransPositionTable.h"
namespace Chess {

    TransPositionTable transPositionTable{};

    size_t TransPositionTable::TRANSPOSITION_TABLE_SIZE = 1'000'000'000;

    TransPositionTable::Entry &TransPositionTable::Bucket::entryToReplace() {
        Entry* worstEntry = entries;
        unsigned int worstEntryImportance = worstEntry->getImportance();

        for (int i = 1; i < BUCKET_SIZE; ++i) {
            Entry &entry = entries[i];
            if (entry.getImportance() < worstEntryImportance) {
                worstEntry = &entry;
            }
        }
        return *worstEntry;
    }

    Chess::TransPositionTable::TransPositionTable(int log2size) {
        numBuckets = 1u << log2size;
        hashMask = firstBits(log2size);
        buckets = std::unique_ptr<Bucket[]>(new Bucket[numBuckets]);
        clear();
    }

    Chess::TransPositionTable::Entry &Chess::TransPositionTable::probe(Chess::Key key, bool &found) {
        Bucket &bucket = bucketOf(key);
        for (Entry &entry: bucket.entries) {
            if(entry.isUninitialized()){
                found = false;
                numEntriesUsed++;
                return entry;
            }
            if (entry.key() == key) {
                found = true;
                return entry;
            }
        }
        numEntriesDeleted++;
        found = false;
        return bucket.entryToReplace();
    }

    ostream &operator<<(ostream &os, const TransPositionTable::Entry &entry) {
        os << "boundType: " <<  static_cast<int>(entry.boundType_) << " bestMove: " << Move(entry.bestMove_) << " score: " << static_cast<int>(entry.score_)
           << " depth: " << static_cast<int>(entry.depth_) << " key: " << entry.key_ << " moveCount_: " << static_cast<int>(entry.moveCount_)
           << " numVisits: " << static_cast<int>(entry.numVisits_);
        return os;
    }

    TransPositionTable TransPositionTable::fromSize(size_t size) {
        std::cout << "Calling from size\n";
        size_t bucketSize = sizeof(Bucket);
        size_t numBuckets = size/bucketSize;
        int log2size = msb(numBuckets); // we want the number of buckets to be a power of 2 for faster hashing, so we take the largest bit only
        return TransPositionTable(log2size);
    }

    bool TransPositionTable::isEmpty() {
        for (int i=0;i<numBuckets;++i) {
            if(!buckets[i].isEmpty()){
                return false;
            }
        }
        return true;
    }
}