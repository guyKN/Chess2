//
// Created by guykn on 1/11/2021.
//

#include "TransPositionTable.h"
namespace Chess {
    TransPositionTable transPositionTable{};

    Chess::TransPositionTable::Entry &Chess::TransPositionTable::Bucket::freeEntry() {
        Entry *worstEntry = &entries[0];
        if (worstEntry->isUninitialized()) {
            return *worstEntry;
        }
        unsigned int worstEntryImportance = worstEntry->getImportance();
        for (int i = 1; i < BUCKET_SIZE; ++i) {
            Entry &entry = entries[i];
            if (entry.isUninitialized()) {
                return entry;
            }
            if (entry.getImportance() < worstEntryImportance) {
                worstEntry = &entry;
            }
        }
        return *worstEntry;
    }

    Chess::TransPositionTable::TransPositionTable(int log2size) {
        numBuckets = 1u << log2size;
        hashMask = firstBits(log2size);
        buckets = std::unique_ptr<Bucket[]>(new Bucket[numBuckets]{});
    }

    Chess::TransPositionTable::Entry &Chess::TransPositionTable::probe(Chess::Key key, bool &found) {
        Bucket &bucket = bucketOf(key);
        for (Entry &entry: bucket.entries) {
            if (entry.key() == key && !entry.isUninitialized()) {
                //todo: also check for uninitialized buckets, since if there's an uninitialized bucket it may be
                // faster to return it immediately, rather than looping around for it
                // also check if the 2 loops can be completly merged
                found = true;
                return entry;
            }
        }
        found = false;
        return bucket.freeEntry();
    }
}