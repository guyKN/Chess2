//
// Created by guykn on 1/11/2021.
//

#ifndef CHESS_TRANSPOSITION_TABLE_H
#define CHESS_TRANSPOSITION_TABLE_H

#include <ostream>
#include "Bitboards.h"
#include "Move.h"
#include <climits>
#include <memory>

namespace Chess {

    class TransPositionTable {
    public:

        static size_t TRANSPOSITION_TABLE_SIZE;

        struct Entry {
            Entry() = default;

        private:
            BoundType boundType_ = BOUND_UNINITIALIZED;
            uint16_t bestMove_{};
            Score score_ = SCORE_ZERO; //todo: store the score as int16_t to save 2 bytes
            uint8_t depth_{};
            Key key_ = KEY_ZERO;
            uint8_t moveCount_ = 0; // the number of half moves at the root of the search when this entry was last used
            uint16_t numVisits_ = 0; // the number of times this position was visited, resets to zero every bestMove.
        public:
            bool operator==(const Entry &rhs) const {
                return boundType_ == rhs.boundType_ &&
                       bestMove_ == rhs.bestMove_ &&
                       score_ == rhs.score_ &&
                       depth_ == rhs.depth_ &&
                       key_ == rhs.key_ &&
                       moveCount_ == rhs.moveCount_ &&
                       numVisits_ == rhs.numVisits_;
            }

            bool operator!=(const Entry &rhs) const {
                return !(rhs == *this);
            }

            friend ostream &operator<<(ostream &os, const Entry &entry);

        public:

            inline bool isUninitialized() const {
                return depth_ == 0;
            }

            inline bool isCurrentlySearched() const {
                return boundType_ & BOUND_CURRENTLY_SEARCHING;
            }

            inline void startSearching(){
                assert(!isCurrentlySearched());
                boundType_^=BOUND_CURRENTLY_SEARCHING;
            }

            inline void stopSearching(){
                assert(isCurrentlySearched());
                boundType_ = static_cast<BoundType>(boundType_ & ~BOUND_CURRENTLY_SEARCHING);
            }

            inline Move bestMove() const {
                return Move(bestMove_); //todo: ensure the rest of the bits are set to zero
            }

            inline void setBestMove(Move move){
                bestMove_ = move.getCode();
            }

            inline Score score() const {
                return score_;
            }

            inline void setScore(Score score){
                score_ = score;
            }

            inline bool isUpperBound() const {
                return boundType_ & BOUND_UPPER;
            }

            inline bool isLowerBound() {
                return boundType_ & BOUND_LOWER;
            }

            inline bool isExact() const {
                return boundType_ == BOUND_EXACT;
            }

            inline void setBoundType(BoundType boundType){
                boundType_ = boundType;
            }

            inline int depth() const {
                return depth_;
            }

            inline void setDepth(unsigned int depth){
                depth_ = depth;
            }

            inline Key key() const {
                return key_;
            }

            inline void setKey(Key key){
                key_ = key;
            }

            inline BoundType boundType(){
                return boundType_;
            }

            inline unsigned int getImportance() const {
                // represents how important the current entry is, in order to decide which entry to replace
                // todo: play around with the numbers to see which replacement function is best
                /// also check if wether it is a PV node or not is relevant
                if (isCurrentlySearched()){
                    return UINT_MAX; //since we don't want to remove a node currently being searched, it's importance is the maximum posible
                }
                return (static_cast<unsigned int>(moveCount_) << 23) + (numVisits_ << depth_);
            }

            inline void onVisit(unsigned int moveCount) {
                if (moveCount_ == moveCount) {
                    numVisits_++;
                } else {
                    numVisits_ = 1;
                }
                moveCount_ = moveCount;
            }

            Entry(Entry &) = delete;

            Entry(Entry &&) = delete;

            Entry &operator=(Entry &) = delete;

            Entry &operator=(Entry &&) = delete;
        };

    private:

        struct Bucket {
            static constexpr int BUCKET_SIZE = 4;
            Entry entries[BUCKET_SIZE] = {};

            Bucket() = default;

            /// if all there is an unused, returns that, otherwise returns the entry with the lowest value
            Entry &entryToReplace();

            Bucket(Bucket &) = delete;

            Bucket(Bucket &&) = delete;

            Bucket &operator=(Bucket &) = delete;

            Bucket &operator=(Bucket &&) = delete;
        };

        int64_t numEntriesUsed = 0;
        int64_t numEntriesDeleted = 0;

        size_t numBuckets;
        Key hashMask;

        std::unique_ptr<Bucket[]> buckets;

        inline Key hash(Key key) {
            return key & hashMask;
        };

        inline Bucket &bucketOf(Key key) {
            return buckets[hash(key)];
        }

    public:
        explicit TransPositionTable() {
            numBuckets = 0;
            hashMask = KEY_ZERO;
            buckets = nullptr;
        }

        explicit TransPositionTable(int log2size);

        static TransPositionTable fromSize(size_t size);

        inline bool isInitialized() const{
            return numBuckets != 0;
        }

        Entry &probe(Key key, bool &found);

        inline size_t getNumEntries() const{
            return numBuckets * Bucket::BUCKET_SIZE;
        }

        inline uint64_t perMillEntriesUsed() const{
            return (numEntriesUsed*1000)/getNumEntries();
        }

        inline uint64_t entriesDeleted() const{
            return numEntriesDeleted;
        }
    };

    extern TransPositionTable transPositionTable;

}
#endif //CHESS_TRANSPOSITION_TABLE_H