#pragma once
#include "manolib.h"

class Transaction{
private:
string transaction_id, sender, receiver;
double amount;

public:
Transaction(string t, string s, string r, double a): 
        transaction_id(t), sender(s), receiver(r), amount(a) {}

    ~Transaction()= default;

    string getTransaction_id()const{return transaction_id;}
    string getSender()const{return sender;}
    string getReceiver()const{return receiver;}
    double getAmount()const{return amount;}

    
};