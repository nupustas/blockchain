#pragma once
#include "manolib.h"

class Transaction {
private:
    string transaction_id;
    string sender;//senders public key
    string receiver;
    double amount;
    bool verified;//true only when block is mined

public:
    // constructor, verified defaults to false
    Transaction(const string &t, const string &s, const string &r, double a, bool v = false)
        : transaction_id(t), sender(s), receiver(r), amount(a), verified(v) {}

    ~Transaction() = default;

    // getters
    const string& getTransaction_id() const { return transaction_id; }
    const string& getSender() const { return sender; }
    const string& getReceiver() const { return receiver; }
    double getAmount() const { return amount; }
    bool isVerified() const { return verified; }

    // setter
    void setVerified(bool v) { verified = v; }
};
