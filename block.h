#pragma once
#ifndef BLOCK_H
#define BLOCK_H

#include "manolib.h"
#include "transaction.h"
#include "functions.h"

class Block {
private:
    int index;
    std::string previous_hash;
    std::string merkle_root;
    uint64_t nonce;
    uint64_t extraNonce;   
    uint64_t timestamp;
    std::vector<Transaction> transactions;
    std::string block_hash;

public:
    // construct a block from given transactions 
    Block(int id, const std::string &prev_hash, const vector<Transaction> &txs, size_t maxTx = 100)
        : index(id),
          previous_hash(prev_hash),
          nonce(0),
          extraNonce(0), 
          timestamp(static_cast<uint64_t>(time(0)))
    {
        // copy up to maxTx transactions
        size_t take = std::min(maxTx, txs.size());
        transactions.assign(txs.begin(), txs.begin() + take);

        merkle_root = computeMerkleRoot(transactions);
        block_hash = headerHash();
    }

    ~Block() = default;

    // merkle root of transactions to "block"
    static string computeMerkleRoot(const vector<Transaction> &txs) {
        if (txs.empty()) return hashas("");

        vector<string> hashes;
        hashes.reserve(txs.size());
        for (const auto &tx : txs) {
            ostringstream ss;
            ss << tx.getTransaction_id() << tx.getSender() << tx.getReceiver() << tx.getAmount();
            hashes.push_back(hashas(ss.str()));
        }

        while (hashes.size() > 1) {
            if (hashes.size() % 2 == 1)
                hashes.push_back(hashes.back()); // duplicate last if odd
            vector<string> next;
            next.reserve(hashes.size() / 2);
            for (size_t i = 0; i < hashes.size(); i += 2) {
                string concat = hashes[i] + hashes[i + 1];
                next.push_back(hashas(concat));
            }
            hashes.swap(next);
        }

        return hashes.front();
    }

    // block header hash
    std::string headerHash() const {
        ostringstream ss;
        ss << previous_hash << merkle_root << timestamp << nonce << extraNonce << index;
        return hashas(ss.str());
    }

    // mine until hash meets difficulty 
    bool mine(int difficulty) {
        if (difficulty < 0) difficulty = 0;
        string target(difficulty, '0');
        int iter = 0;
        const uint64_t MAX_NONCE = std::numeric_limits<uint64_t>::max();

        while (true) {
            block_hash = headerHash();

            // check if hash starts with x zeros
            if (block_hash.rfind(target, 0) == 0) {
                cout << "Block mined! "<< endl;
                return true;
            }

            ++nonce;
            ++iter;

            if (nonce >= MAX_NONCE) {
                nonce = 0;
                ++extraNonce;
            }

        }
    }

    // getters
    const string &getPreviousHash() const { return previous_hash; }
    const string &getMerkleRoot() const { return merkle_root; }
    const string &getHash() const { return block_hash; }
    uint64_t getNonce() const { return nonce; }
    uint64_t getExtraNonce() const { return extraNonce; }
    uint64_t getTimestamp() const { return timestamp; }
    const vector<Transaction> &getTransactions() const { return transactions; }
    int getIndex() const { return index; }
};

#endif 
