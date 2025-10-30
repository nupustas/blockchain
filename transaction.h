#pragma once
#include "manolib.h"

class Transaction {
private:
    string transaction_id;
    string sender;
    string receiver;
    double amount;
    bool verified;

public:
    // constructor - verified defaults to false
    Transaction(const string &t, const string &s, const string &r, double a, bool v = false)
        : transaction_id(t), sender(s), receiver(r), amount(a), verified(v) {}

    ~Transaction() = default;

    // getters
    const std::string& getTransaction_id() const { return transaction_id; }
    const std::string& getSender() const { return sender; }
    const std::string& getReceiver() const { return receiver; }
    double getAmount() const { return amount; }
    bool isVerified() const { return verified; }

    // setter
    void setVerified(bool v) { verified = v; }
};
